#include "proIso15765_2.h"
#define MAX_BUFFER_SIZE 4294967295

proIso15765_2::proIso15765_2()
{
	m_linkConfirm = NULL;
	m_linkRequest = NULL;
	m_linkIndication = NULL;
	m_threadRun = false;
}

proIso15765_2::~proIso15765_2()
{
}

//1. Judge frame type and format to single or multi frame.
//2. Transfer single frame or multiframe
//3. Add result EventList;
bool proIso15765_2::request(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr, u8* sendBuf, u32 sendLen)
{
	ADDR_INFO addrInfo = ADDR_INFO(srcAddr, tgtAddr, hasExtAddr, extAddr);
	if (sendLen > MAX_BUFFER_SIZE)
	{
		addEvent(RESULT::error,addrInfo);
		return false;
	}

	PDU pdu;
	if (!packData(addrInfo, sendBuf, sendLen, &pdu))
	{
		addEvent(RESULT::error,addrInfo);
		return false;
	}
	
	m_requestDataList.push_back(pdu);

	return true;
}

u32 proIso15765_2::firstFrameIndication(u8* srcAddr, u8* tgtAddr, bool* hasExtAddr, u8* extAddr, RESULT* pResult)
{
	u32 ret = 0;
	
	for (std::unordered_map<ADDR_INFO, PDU>::const_iterator it = m_recvMap.begin(); it != m_recvMap.end(); ++it)
	{
		PDU pdu = it->second;
		if (pdu.procolCtrlInfo==PROTOCOL_CONTROL_INFO::firstFrame)
		{
			ret = pdu.dataList.size();
			ADDR_INFO addrInfo = it->first;
			*srcAddr = addrInfo.srcAddr;
			*tgtAddr = addrInfo.tgtAddr;
			*hasExtAddr = addrInfo.withExt;
			*extAddr = addrInfo.extAddr;
			*pResult = RESULT::ok;
			return ret;
		}
	}
	*pResult = RESULT::error;
	return ret;
}

proIso15765_2::RESULT proIso15765_2::indication(u8* srcAddr, u8* tgtAddr, bool* hasExtAddr, u8* extAddr, u8* recvBuf, u32* recvLen)
{
	
	for (auto it = m_recvMap.begin(); it != m_recvMap.end(); ++it)
	{
		auto addrInfo = it->first;
		*srcAddr = addrInfo.srcAddr;
		*tgtAddr = addrInfo.tgtAddr;
		*hasExtAddr = addrInfo.withExt;
		*extAddr = addrInfo.extAddr;
		PDU pdu = it->second;
		std::copy(pdu.dataList.begin(), pdu.dataList.end(), recvBuf);
		*recvLen = pdu.dataList.size();
	}
	return RESULT::ok;
}

proIso15765_2::RESULT proIso15765_2::confirm(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr)
{
	ADDR_INFO adinfo = ADDR_INFO(srcAddr, tgtAddr, hasExtAddr, extAddr);
	std::unordered_map<ADDR_INFO, RESULT>::const_iterator got = m_eventMap.find(adinfo);
	if (got!=m_eventMap.end())
	{
		return got->second;
	}
	return RESULT::error;
}

void proIso15765_2::changeParameterRequest(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr, PARAM_TYPE type, u8 value)
{	
	RESULT_CHANGE_PARAM result = RESULT_CHANGE_PARAM::defaultValue;
	if (type != stMin && type != blockSize) result = RESULT_CHANGE_PARAM::wrongParam;
	ADDR_INFO addrInfo = ADDR_INFO(srcAddr, tgtAddr, hasExtAddr, extAddr);
	PARAM param(type, value, result);
	m_paramMap.insert(std::make_pair(addrInfo, param));
}

proIso15765_2::RESULT_CHANGE_PARAM proIso15765_2::changeParameterConfirm(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr, PARAM_TYPE param)
{
	std::unordered_map<ADDR_INFO, PARAM>::const_iterator it = m_paramMap.find(ADDR_INFO(srcAddr, tgtAddr, hasExtAddr, extAddr));
	if (it!= m_paramMap.end())
		return it->second.res;
	return RESULT_CHANGE_PARAM::defaultValue;
}

//{ m_canIDList[ADDR_INFO(srcAddr, tgtAddr, hasExtAddr, extAddr)] = canID; };
void proIso15765_2::setCanID(u8 srcAddr, u8 tgtAddr, bool hasExtAddr, u8 extAddr, u32 canID, bool is11bit, bool isCanFD)
{

}

void proIso15765_2::setLinkLayer(LinkRequest* pReq, LinkConfirm* pCon, LinkIndication* pInd)
{
	m_linkRequest = pReq;
	m_linkConfirm = pCon;
	m_linkIndication = pInd;
}

//Single frame 
bool proIso15765_2::transferUnsegmentData()
{
	//Call 11898_1 to transfer cmd data;
	for (std::list<PDU>::iterator it = m_requestDataList.begin();it!= m_requestDataList.end(); ++it)
	{
		if (m_linkRequest == NULL)
			return false;

		PDU pdu = *it;
		u8* buf = new u8[pdu.dataList.size()];
		CAN_ADDR_INFO canAddrInfo = m_canIDList.at(pdu.addrInfo);
		return (*m_linkRequest)(canAddrInfo.canID, canAddrInfo.formatType, canAddrInfo.getDataLenCode(pdu.dataList.size()), pdu.getBuf(buf));
	}
	return false;
}

//Multi frame
bool proIso15765_2::transferSegmentData()
{
	return false;
}

//return : 
bool proIso15765_2::packData(ADDR_INFO info, u8* buf, u32 len, PDU* pdu)
{
	return false;
}

void proIso15765_2::threadWrok()
{
	if (!m_threadRun)
		return;
	if (m_requestDataList.size() > 0)
	{
		processSend();
	}
	
	processRecv();
}

void proIso15765_2::processSend()
{
}

void proIso15765_2::processRecv()
{
}

