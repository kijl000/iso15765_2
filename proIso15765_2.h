#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <thread>
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char

class proIso11898_1;
//The service primitive requests transmission of <Data> that shall be mapped within specific attributes
//of the data link protocol data unit selected by means of <Identifier>.
//The <Identifier> shall provide reference to the specific addressing format used to transmit <Data>:
bool (proIso11898_1::* ptRequest)(u32 canID, u8 frameFormatType, u8 dataLenCode, u8* data) = NULL;

//The service primitive confirms the completion of an L_Data.request service for a specific <Identifier>.
void (proIso11898_1::* ptConfirm)(u32 canID, u8 transStatus) = NULL;

//The service primitive indicates a data link layer event to the adjacent upper layer and delivers <Data>
//identified by <Identifier> :
u32(proIso11898_1::* ptIndication)(u32* canID, u8* frameFormatType, u8* dataLenCode, u8* data) = NULL;

class proIso15765_2
{
public:
    //8.3.2.4 N_TAtype, Network target address type
    //Type : enumeration
	//frameFormatType
	typedef enum _TA_TYPE
	{
		tgtAddrTypePhyClassic11bit,
		tgtAddrTypeFuncClassic11bit,
		tgtAddrTypePhyFD11bit,
		tgtAddrTypeFuncFD11bit,
		tgtAddrTypePhyClassic29bit,
		tgtAddrTypeFuncClassic29bit,
		tgtAddrTypePhyFD29bit,
		tgtAddrTypeFuncFD29bit
	}TARGET_ADDRESS_TYPE,FRAME_FORMAT_TYPE;

	//This parameter contains the status relating to the outcome of a service execution. If two
	//or more errors are discovered at the same time, then the network layer entity shall use the parameter
	//value found first in this list when indicating the error to the higher layers.
	typedef enum class _N_RESULT
	{
		//This value means that the service execution has been completed successfully; it can be issued to a
		//service user on both the senderand receiver sides.
		ok,
		//This value is issued to the protocol user when the timer N_Ar / N_As has passed its time - out value
		//N_Asmax /N_Armax; it can be issued to service users on both the senderand receiver sides.
		//Ar: Time for transmission of the CAN frame(any N_PDU) on the receiver side
		//As: Time for transmission of the CAN frame(any N_PDU) on the sender side
		timeoutA,
		//This value is issued to the service user when the timer N_Bs has passed its time - out value N_Bsmax;
	    //it can be issued to the service user on the sender side only.
		//Bs:Time until reception of the next FlowControl N_PDU
		timeoutBs,
		//This value is issued to the service user when the timer N_Cr has passed its time - out value N_Crmax;
	    //it can be issued to the service user on the receiver side only.
		//Cr:Time until reception of the next Consecutive Frame N_PDU
		timeoutCr,
		//This value is issued to the service user upon receipt of an unexpected SequenceNumber (PCI.SN)
		//value; it can be issued to the service user on the receiver side only.
		wrongSN,
		//This value is issued to the service user when an invalid or unknown FlowStatus value has been
		//received in a FlowControl(FC) N_PDU; it can be issued to the service user on the sender side only.
		invalidFS,
		//This value is issued to the service user upon receipt of an unexpected protocol data unit; it can be
		//issued to the service user on the receiver side only.
		unexpPDU,
		/*This value is issued to the service user when the receiver has transmitted N_WFTmax FlowControl
		N_PDUs with FlowStatus = WAIT in a row and following this, it cannot meet the performance
		requirement for the transmission of a FlowControl N_PDU with FlowStatus = ClearToSend.It can be
		issued to the service user on the receiver side only.*/
		wftOVRN,
        /*This value is issued to the service user upon receipt of a FlowControl(FC) N_PDU with
		FlowStatus = OVFLW.It indicates that the buffer on the receiver side of a segmented message
		transmission cannot store the number of bytes specified by the FirstFrame DataLength(FF_DL)
		parameter in the FirstFrame and therefore the transmission of the segmented message was
		aborted.It can be issued to the service user on the sender side only.*/
		bufferOverFlW,
		/*This is the general error value.It shall be issued to the service user when an error has been detected
		by the network layerand no other parameter value can be used to better describe the error.It can
		be issued to the service user on both the senderand receiver sides.*/
		error
	}*pRESULT,RESULT;

	typedef enum _PARAM_TYPE
	{
		 stMin,
		 blockSize,
		 defaultValue
	}PARAM_TYPE;
	
	typedef enum class _RESULT_CHANGE_PARAM
	{
		//This value means that the service execution has been completed successfully; it can be issued to a
		//service user on both the senderand receiver sides.
		ok,
		//This value is issued to the service user to indicate that the service did not execute since reception
		//of the message identified by <N_AI> was taking place; it can be issued to the service user on the
		//receiver side only.
		rxOn,
		//This value is issued to the service user to indicate that the service did not execute due to an
		//undefined <Parameter>; it can be issued to a service user on both the receiverand sender sides.
		wrongParam,
		//This value is issued to the service user to indicate that the service did not execute due to an out-ofrange
		//<Parameter_Value>; it can be issued to a service user on both the receiverand sender sides.
		wrongValue,
		
		defaultValue

	}RESULT_CHANGE_PARAM;

public:
	//7.2 Services provided by network layer to higher layers
	//a) Communication services:

	//request: This service is used to request the transfer of data. If necessary, the network layer segments the data.
	bool request(u8 srcAddr,u8 tgtAddr, bool hasExtAddr,u8 extAddr, u8* sendBuf, u32 sendLen)  ;

	//indication: This service is used to signal the beginning of a segmented message reception to the upper layer.
	u32 firstFrameIndication(u8*srcAddr,u8*tgtAddr,bool*hasExtAddr, u8*extAddr, RESULT* pResult);

	//This service is used to provide received data to the higher layers
	RESULT indication(u8* srcAddr, u8* tgtAddr, bool* hasExtAddr, u8* extAddr, u8* recvBuf, u32* recvLen);

	//This service confirms to the higher layers that the requested service has been carried out(successfully or not).
	RESULT confirm(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr) ;


	//b) Protocol parameter setting services:
	//These services, of which the following are defined, enable the dynamic setting of protocol
	//parameters.

	//ChangeParameter.request:
	//service is used to request the dynamic setting of specific
	//internal parameters.
	void changeParameterRequest(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr, PARAM_TYPE param, u8 paramValue);

	//This service confirms to the upper layer that the request to
	//change a specific protocol has completed(successfully or not).
	RESULT_CHANGE_PARAM changeParameterConfirm(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr, PARAM_TYPE param);

public:
	void setCanID(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr, u32 canID, bool is11bit, bool isCanFD);

	void set11898Protocol(proIso11898_1* pPro11898) { m_11898_1 = pPro11898; }
	
private:
	//9 Transport layer protocol
	//9.6 Protocol control information specification
	typedef enum 
	{
		singleFrame,

		firstFrame,

		consecutiveFrame,

		flowControlFrame,

		resverd

	} PROTOCOL_CONTROL_INFO;


	typedef struct _DATA_ADDR
	{
		u8 srcAddr;
		u8 tgtAddr;
		bool withExt;
		u8 extAddr;
		
	public:
		_DATA_ADDR() {
			srcAddr = 0;
			tgtAddr = 0;
			withExt = false;
			extAddr = 0;
		
		};
		_DATA_ADDR(u8 src, u8 tgt, bool _withExt, u8 _extAddr) {
			srcAddr = src;
			tgtAddr = tgt;
			withExt = _withExt;
			extAddr = _extAddr;
		

		};

	}ADDR_INFO;

	typedef struct
	{
		ADDR_INFO addrInfo;
		PROTOCOL_CONTROL_INFO procolCtrlInfo;
		std::list<u8> dataList;
	public :
		u8* getBuf(u8* out)
		{
			std::copy(dataList.begin(), dataList.end(), out);
			return out;
		};
	}PDU;

	typedef struct  _PARAM
	{
		ADDR_INFO addrInfo;
		PARAM_TYPE type;
		u8 value;
		RESULT_CHANGE_PARAM res;
	public :
		_PARAM() { addrInfo = ADDR_INFO(); type = PARAM_TYPE::defaultValue; value = 0; res = RESULT_CHANGE_PARAM::defaultValue; };
		_PARAM(ADDR_INFO addrInfo, PARAM_TYPE _type, u8 _value, RESULT_CHANGE_PARAM result) { addrInfo = addrInfo; type = _type; value = _value; res = result; };
	}PARAM;

	typedef struct _CAN_ID
	{
		u32 canID;
		FRAME_FORMAT_TYPE formatType;
	public:
		_CAN_ID(u32 _canID, bool is11bit, bool isPhy = true, bool isCanClassic = false)
		{
			canID = _canID;

			if (is11bit)
			{
				if (isPhy)
					formatType = isCanClassic? tgtAddrTypePhyClassic11bit:tgtAddrTypePhyFD11bit;
				else //isFunc
					formatType = isCanClassic ? tgtAddrTypeFuncClassic11bit : tgtAddrTypeFuncFD11bit;
			}
			else //29bit
			{
				if(isPhy)
					formatType = isCanClassic ? tgtAddrTypePhyClassic29bit : tgtAddrTypePhyFD29bit;
				else //isFunc
					formatType = isCanClassic ? tgtAddrTypeFuncClassic29bit : tgtAddrTypeFuncFD29bit;
			}
		};

		u8 getDataLenCode(u32 dataSize) 
		{
			//todo judge canlen;
			return 0;
		};
	}CAN_ADDR_INFO;
private:

	//7.3 Internal operation of network layer
	bool transferUnsegmentData();

	//Multi frame request;
	bool transferSegmentData();

	void addEvent(RESULT result, ADDR_INFO addrInfo) { m_eventMap[addrInfo] = result; };

	//Pack raw data to pdu
	bool packData(ADDR_INFO info, u8* buf, u32 len, PDU* pdu);

	void threadWrok();

	void processSend();

	void processRecv();

private:

	std::unordered_map<ADDR_INFO, RESULT> m_eventMap;

	//The request data list waiting tobe transfered to the CANBUS
	std::list<PDU> m_requestDataList;

	//The recv data map waiting to be transfered to the upper level
	std::unordered_map<ADDR_INFO, PDU> m_recvMap;

	//The param map can be changed by the upper level 
	std::unordered_map<ADDR_INFO, PARAM> m_paramMap;

	//Send work thread
	std::thread sendThread;

	//Recv work thread
	std::thread recvThread;

	//CAN 11bit or 29bit ID
	//<addrInfoHashCode, CANID>
	std::unordered_map<ADDR_INFO, CAN_ADDR_INFO> m_canIDList;

	proIso11898_1* m_11898_1;

	bool m_threadRun;

};


