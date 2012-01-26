// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
//
//  Versão reescrita do emulador de JVS, com suporte a multiplos
// comandos por stream.


#include "stdafx.h"
#include <queue>

#define JVS_TRUE				0x01
#define JVS_FALSE				0x00
#define JVS_SYNC_CODE			0xE0
#define JVS_REPORT_CODE			0x01
#define JVS_COMMAND_REV			0x13
#define JVS_BOARD_REV			0x30
#define JVS_COMM_REV			0x10
#define JVS_ADDR_MASTER			0x00
#define JVS_STATUS_OK			0x01
#define JVS_STATUS_UNKNOWN_CMD	0x02
#define JVS_STATUS_SUM_ERROR	0x03
#define JVS_STATUS_ACK_OVERFLOW	0x04
#define JVS_REPORT_OK			0x01
#define JVS_REPORT_INVAL_PARAM1	0x02
#define JVS_REPORT_INVAL_PARAM2	0x03
#define JVS_REPORT_BUSY			0x04
#define JVS_STREAM_SIZE			1024

#define JVS_OP_RESET			0xF0
#define JVS_OP_ADDRESS			0xF1
#define JVS_OP_CHECKSUM_ERROR	0x2F

#define JVS_IOFUNC_SWINPUT		0x01
#define JVS_IOFUNC_COINTYPE		0x02
#define JVS_IOFUNC_EXITCODE		0x00

#define JVS_SUPPORT_PLAYERS		2
#define JVS_SUPPORT_SLOTS		2

#define LOG_COMMANDS	1
#if LOG_COMMANDS
#define logcmd	logmsg
#else
#define logcmd(str, __VA_LIST__)
#endif

typedef signed char SINT8;
typedef signed short SINT16;
typedef signed short SINT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
//typedef unsigned short UINT32;

static const char *IO_Id = "NAMCO LTD.;I/O PCB-1000;ver1.0;for domestic only,no analog input";

/*
* Para cada pacote recebido temos:
*
* +-----------+---------------+------+-------+-------+----------+
* | SYNC_CODE | SLAVE_ADDRESS | SIZE | FUNC1 | FUNCn | CHECKSUM |
* +-----------+---------------+------+-------+-------+----------+
*
* Para cada pacote enviado temos:
*
* +-----------+--------------------+------+--------+---------+---------+----------+
* | SYNC_CODE | MASTER_ADDRESS (0) | SIZE | STATUS | REPORT1 | REPORTn | CHECKSUM |
* +-----------+--------------------+------+--------+---------+---------+----------+
* 
* Para cada função recebida é necessário um REPORT, depois do report temos os valores
* da resposta do comando.
*/

struct jvs_command_def {
	UINT8 params;
	UINT8 reports;
};

class InputInfo {
public:
	InputInfo() {
	}
	~InputInfo() {
	}
	/*
	hi bits:

	0 - BUT 2
	1 - BUT 1
	2 - LEFT
	3 - RIGHT
	4 - DOWN
	5 - UP
	6 - SERVICE
	7 - 
	8 - START

	lo bits
	0 -
	1 -
	2 -
	3 -
	4 - BUT 6
	5 - BUT 5
	6 - BUT 4
	7 - BUT 3
	*/

	BYTE Xp1HiByte()
	{
		BYTE s = 0;

		if (inputMgr.GetState(P1_START))
			s |= 0x80;
		if (inputMgr.GetState(P1_SERVICE))
			s |= 0x40;


		if (inputMgr.GetState(P1_UP))
			s |= 0x20;
		if (inputMgr.GetState(P1_DOWN))
			s |= 0x10;
		if (inputMgr.GetState(P1_RIGHT))
			s |= 0x04;
		if (inputMgr.GetState(P1_LEFT))
			s |= 0x08;
		if (inputMgr.GetState(P1_BUTTON_1))
			s |= 0x02;
		if (inputMgr.GetState(P1_BUTTON_2))
			s |= 0x01;
		return s;
	}

	BYTE Xp1LoByte()
	{
		BYTE s = 0;
		if (inputMgr.GetState(P1_BUTTON_3))
			s |= 0x80;
		if (inputMgr.GetState(P1_BUTTON_4))
			s |= 0x40;
		if (inputMgr.GetState(P1_BUTTON_5))
			s |= 0x20;
		if (inputMgr.GetState(P1_BUTTON_6))
			s |= 0x10;
		return s;
	}

	BYTE Xp2HiByte()
	{
		BYTE s = 0;

		if (inputMgr.GetState(P2_START))
			s |= 0x80;
		if (inputMgr.GetState(P2_SERVICE))
			s |= 0x40;
		if (inputMgr.GetState(P2_UP))
			s |= 0x20;
		if (inputMgr.GetState(P2_DOWN))
			s |= 0x10;
		if (inputMgr.GetState(P2_RIGHT))
			s |= 0x04;
		if (inputMgr.GetState(P2_LEFT))
			s |= 0x08;
		if (inputMgr.GetState(P2_BUTTON_1))
			s |= 0x02;
		if (inputMgr.GetState(P2_BUTTON_2))
			s |= 0x01;
		return s;
	}

	BYTE Xp2LoByte()
	{
		BYTE s = 0;
		if (inputMgr.GetState(P2_BUTTON_3))
			s |= 0x80;
		if (inputMgr.GetState(P2_BUTTON_4))
			s |= 0x40;
		if (inputMgr.GetState(P2_BUTTON_5))
			s |= 0x20;
		if (inputMgr.GetState(P2_BUTTON_6))
			s |= 0x10;
		return s;
	}


};

InputInfo inpInfo;




#define __ARG__(n)	((DWORD) pfunc[n])



class jprot_encoder {
	BYTE buffer[1024];
	BYTE *ptr;
	DWORD statusaddr;
	DWORD sizeaddr;
	DWORD sumaddr;
	DWORD startaddr;
	DWORD xpos;
	DWORD size_;
	DWORD nreplys;
public:
	DWORD size() {
		return size_;
	}
	jprot_encoder() {
		memset(buffer, 0, 1024);
		xpos = 0;
	}
	~jprot_encoder() {
		memset(buffer, 0, 1024);
		xpos = 0;
	}

	void addreply() {
		nreplys++;
	}


	void set_status(BYTE v) {
		buffer[statusaddr] = v;
	}
	void clear()
	{
		memset(buffer, 0, 1024);
		size_ = 0;
		xpos = 0;
	}
	void begin_stream() {

		pushz(JVS_SYNC_CODE);
		sumaddr = xpos;
		pushz(JVS_ADDR_MASTER);
		sizeaddr = xpos;
		push(0);
		statusaddr = xpos;
		// STATUS
		push(JVS_STATUS_OK);

	}

	void pushz(BYTE v) {
		buffer[xpos++] = v;
	}

	void push(BYTE v) {
#if 0
		buffer[xpos]=v;
		++xpos;
#else
		if(v==0xD0) {
			buffer[xpos++]=0xD0;
			buffer[xpos++]=0xCF;
		} else
		if(v==0xE0) {
			buffer[xpos++]=0xD0;
			buffer[xpos++]=0xDF;
		} else
			buffer[xpos++] = v;
#endif
	}
	void report(BYTE v) {
		push(v);
	}
	void end_stream() {
#if 0
		if (xpos == (statusaddr+1)) {
			clear();
			return;
		}
		// codifica o tamanho da stream
		buffer[sizeaddr] = (BYTE) xpos-sizeaddr;

		// calcula o checksum
		DWORD sum = 0;
		for (DWORD i=sumaddr ; i<xpos; i++) {
			sum += (DWORD) (buffer[i] & 0xFF);
		}
		sum &= 0xFF;
		if ((sum == 0xD0) || (sum == 0xE0)) {
			push(0xD0);
			push(sum-1);
		} else
			push(sum);
		size_ = xpos;
#else
		if (xpos == (statusaddr+1)) {
			clear();
			return;
		}

		DWORD sizeK = 0;
		for (DWORD i=sizeaddr;i<xpos;i++)
			if (buffer[i] != 0xD0)
				++sizeK;
		// codifica o tamanho da stream
		buffer[sizeaddr] = sizeK;

		// calcula o checksum
		DWORD sum = 0;
		for (DWORD i=sumaddr,inc=0 ; i<xpos; i++) {
			if (buffer[i] == 0xD0) {
				inc = 1;
			} else {
				sum += (DWORD) ((buffer[i] + inc) & 0xFF);
				if (inc)
					inc = 0;
			}
		}
		sum &= 0xFF;
		push(sum);
		size_ = xpos;
#endif
	}


	void read(BYTE *dst, DWORD size) {
		if (size > size_)
			size = size_;
		memcpy(dst, &buffer[0], size);
	}

	void print()
	{
		if (size()) {
			logmsg("WR:  ");
			for(DWORD i=0;i<size();i++)
				logmsg("%02X ", buffer[i]);
			logmsg("\n");
		}
	}
};

static int isAddressed = 0;
int is_addressed() {
	return isAddressed;
}
void reset_addressed()
{
	isAddressed=0;
}

static WORD p1coin = 0;
static WORD p2coin = 0;
static int coinstate[2]= {0,0};




DWORD process_stream(UINT8 *stream, DWORD srcsize, BYTE *dst, DWORD dstsize)
{
	jprot_encoder r;
	BYTE *pstr = stream;
	BYTE *pfunc = NULL;
	BYTE node = 0;
	BYTE pktsize = 0;
	int pktcount = 0;
	WORD pktaddr = 0;

	r.clear();


	if (pstr[0] != JVS_SYNC_CODE) {
		logmsg("SYNC_CODE inválido!\n");
	}
	node = pstr[1];
	pktsize = pstr[2];
	pfunc = &pstr[3];

	/* pktcount é o número de bytes disponíveis para a stream de funções */
	pktcount = (int) pktsize - 1;

	r.begin_stream();

	while (pktcount > 0)
	{
		//	logcmd("pktcount = %d\n", pktcount);
		int increment = 0;
		switch (pfunc[0] & 0xFF)
		{

		case 0xF0:

			logcmd("Reset %X\n", __ARG__(1));
			p1coin = 0;
			p2coin = 0;
			increment = 2;
			break;
		case 0xF1:
			logcmd("Address %X\n", __ARG__(1));
			r.report(JVS_REPORT_OK);
			isAddressed = 1;

			increment = 2;
			break;
		case 0x2F:
			logcmd("Erro no checksum!\n");
			increment = 1;
			break;

			// I/O ID
		case 0x10:
			{
				logcmd("Enviando identificação \"%s\"\n", IO_Id);
				r.report(JVS_REPORT_OK);
				const char *str = IO_Id;
				while (*str) { r.push(*str++); }
				r.push(0);
				increment = 1;
				break;
			}

		case 0x11:
			logcmd("Revisão dos comandos %X\n", JVS_COMMAND_REV);
			r.report(JVS_REPORT_OK);
			r.push(JVS_COMMAND_REV);
			increment = 1;
			break;

			// Revisão da placa (JVS)
		case 0x12:
			r.report(JVS_REPORT_OK);
			r.push(JVS_BOARD_REV);
			increment = 1;
			break;

		case 0x13:
			r.report(JVS_REPORT_OK);
			r.push(JVS_COMM_REV);
			increment = 1;
			break;
			// IO REPORT
			// Aqui é onde reportamos as funções suportadas pela JVS
			// através de um subset (JVS_IOFUNC_*) de funções com 3 parâmetros cada uma delas.
		case 0x14:
			r.report(JVS_REPORT_OK); 

			r.push(JVS_IOFUNC_SWINPUT);
			{
				r.push(JVS_SUPPORT_PLAYERS); // 2 players
				r.push(14); // 14 botões
				r.push(0); // null
			}
			r.push(JVS_IOFUNC_COINTYPE);
			{
				r.push(JVS_SUPPORT_SLOTS); // 2 slots
				r.push(0); // null
				r.push(0); // null
			}

			r.push(JVS_IOFUNC_EXITCODE);
			{
				r.push(0);
				r.push(0);
				r.push(0);
			}

			
		
			increment = 1;
			break;


			// Informações sobre o monitor
		case 0x01:
			r.report(JVS_REPORT_OK);
			r.push(configMgr.GetConfig(TTX_CONFIG_HIRES));
			r.push(0);
			increment = 2;
			break;

			// Status dos controles digitais
		case 0x20:
			{
				//logcmd("Lendo controles %X,%X...\n", __ARG__(1), __ARG__(2));
				r.report(JVS_REPORT_OK);
				// test mode, tilt, etc..

				if (inputMgr.GetState(TEST_MODE))
					r.push(0x80);
				else
					r.push(0);

				r.push(inpInfo.Xp1HiByte());
				r.push(inpInfo.Xp1LoByte());
				r.push(inpInfo.Xp2HiByte());
				r.push(inpInfo.Xp2LoByte());

				increment = 3;

				break;
			}

		case 0x21:
			{

				int currstate = inputMgr.GetState(P1_COIN);
				if (!coinstate[0] && (currstate)) {
					++p1coin;
				}
				coinstate[0] = currstate;

				currstate = inputMgr.GetState(P2_COIN);
				if (!coinstate[1] && (currstate)) {
					++p2coin;

				}
				coinstate[1] = currstate;

				//logcmd("Lendo o ficheiro... \n");
				r.report(JVS_REPORT_OK);
				r.push(p1coin >> 8);
				r.push(p1coin & 0xFF);
				r.push(p2coin >> 8);
				r.push(p2coin & 0xFF);
				increment = 2;
				break;
			}

		case 0x30:
			{
				WORD val = ((__ARG__(2) & 0xFF) << 8) | (__ARG__(3) & 0xFF);
				r.report(JVS_REPORT_OK);

				logcmd("-coin %d, %d\n", __ARG__(1), val);

				switch (__ARG__(1))
				{
				case 1:
					if (val > p1coin)
						p1coin = 0;
					else
						p1coin -= val;
					break;
				case 2:
					if (val > p2coin)
						p2coin = 0;
					else
						p2coin -= val;
					break;
				}

				increment = 4;
				break;
			}

		case 0x31:
			{
				
				WORD val = ((__ARG__(2) & 0xFF) << 8) | (__ARG__(3) & 0xFF);
				r.report(JVS_REPORT_OK);

				logcmd("+coin %d, %d\n", __ARG__(1), val);

				switch (__ARG__(1))
				{
				case 1: p1coin += val; break;
				case 2: p2coin += val; break;
				}

				increment = 4;
				break;
			}


		default:
			logcmd("Comando não reconhecido %X\n", __ARG__(0));
			r.report(JVS_REPORT_OK);
			increment = 1;
			break;
		}

		pktcount -= increment;
		pfunc += increment;
	}

	r.set_status(JVS_STATUS_OK);
	r.end_stream();
	//r.print();
	r.read(dst, dstsize);

	return r.size();
}





