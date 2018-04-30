#include "node_util.h"

#define INFO(str, ...)	printf("[INFORMATION] " str "\r\n", ##__VA_ARGS__)
#define TRUE "true"
#define FALSE "false"

#define EXTRA_BYTES 13
#define LBT_MIN_HUGE_TIME_MS 50

#define SEND_DONE 1
#define SENDING 2

bool sending = false;
Timer timer;

void setPower(uint8_t& power) {
    dot->setTxPower(power);
    printf("Set power to %u\r\n", power);
}

void setDataRate(uint8_t& datarate) {
    dot->setTxDataRate(datarate);
    printf("Set datarate to %u\r\n", datarate);
}

void setPacketLength(vector<uint8_t>& data, uint8_t& len) {
    len = (len <= 0) ? 1 : len;
    len = (len > 242) ? 242 :len;
    data.clear();
    for(int i=0; i<len; i++) data.push_back('0');
    printf("Set packet length to %u\r\n", data.size());
}

bool isSending() {
    return sending;
}

uint8_t send(vector<uint8_t>& data) {    
    if(sending) return SENDING;
    sending = true;
    timer.reset();
    timer.start();
    if(dot->send(data) == mDot::MDOT_OK) {
        timer.stop();
        printf("ack=%d\r\n", timer.read_ms());
        //printf("ack=%d\r\n", dot->getRssiStats().last);
    }else {
        printf("ack=NaN\r\n");
        // 수신 실패
    }
    sending = false;
    return SEND_DONE;
}

void linkCheck() {
    mDot::link_check res = dot->networkLinkCheck();
    printf("[Link Check] status=%s(%li) \r\n", res.status ? TRUE : FALSE, res.dBm);
}

void displayInformation() {
    INFO("Library Version ---------------- %s", dot->getId().c_str());
    INFO("MTS LoRa Version --------------- %s", dot->getMtsLoraId().c_str());
    INFO("Log Level ---------------------- %i", dot->getLogLevel());
    INFO("Activity Led ------------------- %s", dot->getActivityLedEnable() ? "Enabled" : "Disabled");
    INFO("ActivityLedPin ----------------- %s", dot->getActivityLedPin() ? TRUE : FALSE);

    for(unsigned int i=0; i<dot->getChannelRanges().size(); i++) {
    INFO("Data Rate Range [%02u] ----------- %u", i, dot->getChannelRanges().at(i));
    }

    for(unsigned int i=0; i<dot->getChannels().size(); i++) {
    INFO("Channal [%02u] ------------------- %lu", i, dot->getChannels().at(i));
    }
    
    for(unsigned int i=0; i<dot->getConfigChannelRanges().size(); i++) {
    INFO("Config Channel [%02u] ------------ %u", i, dot->getConfigChannelRanges().at(i));
    }

    INFO("Frequency Band ----------------- %s", mDot::FrequencyBandStr(dot->getFrequencyBand()).c_str());
    INFO("Channel Plan ------------------- %s", dot->getChannelPlanName().c_str());
    INFO("Session Datarate --------------- %s", mDot::DataRateStr(dot->getSessionDataRate()).c_str());
    INFO("Session Max EIRP --------------- %i", dot->getSessionMaxEIRP());
    INFO("Uplink Dwell Time -------------- %i", dot->getSessionUplinkDwelltime());

    INFO("Public Network ----------------- %s", dot->getPublicNetwork() ? TRUE : FALSE);
    
    //INFO("Device ID ---------------------- %s", dot->getDeviceId());
    INFO("App Port ----------------------- %i", dot->getAppPort());
    INFO("Device Class ------------------- %s", dot->getClass().c_str());
    INFO("Max Packet Length -------------- %i", dot->getMaxPacketLength());
    //INFO("Network Address -------------- %s", dot->getNetworkAddress());
    //INFO("Network Session Key ---------- %s", dot->getNetworkSessionKey());
    //INFO("Data Session Key ------------- %s", dot->getDataSessionKey());
    INFO("Network Name ------------------- %s", dot->getNetworkName().c_str());
    //INFO("Network ID ------------------- %s", dot->getNetworkId());
    INFO("Network Passphrase ------------- %s", dot->getNetworkPassphrase().c_str());
    //INFO("Network Key ------------------ %s", dot->getNetworkKey());
    //INFO("LoRaWAN App EUI -------------- %s", dot->getAppEUI());
    INFO("Join Byte Order ---------------- %s", dot->getJoinByteOrder() ? "LSB" : "MSB");
    INFO("Join Status -------------------- %s", dot->getNetworkJoinStatus() ? "Joined" : "Not Joined");
    INFO("Join Delay --------------------- %i sec", dot->getJoinDelay());
    INFO("Join Rx1 DR Offset ------------- %u", dot->getJoinRx1DataRateOffset());
    INFO("Join Rx2 Data Rate ------------- %u", dot->getJoinRx2DataRate());
    INFO("Join Rx2 Frequency ------------- %lu", dot->getJoinRx2Frequency());
    INFO("Join Mode ---------------------- %s", mDot::JoinModeStr(dot->getJoinMode()).c_str());
    INFO("Join Retries ------------------- %i", dot->getJoinRetries());

    INFO("Rx Delay ----------------------- %u", dot->getRxDelay());
    INFO("Preserve Session --------------- %s", dot->getPreserveSession() ? "Enabled" : "Disabled");

    INFO("Link Check Count --------------- %u", dot->getLinkCheckCount());
    INFO("Link Check Threshold ----------- %u", dot->getLinkCheckThreshold());
    INFO("Link Fail COunt ---------------- %u", dot->getLinkFailCount());
    INFO("Uplink Counter ----------------- %lu", dot->getUpLinkCounter());
    INFO("Downlink Counter --------------- %lu", dot->getDownLinkCounter());

    INFO("AES Encryption ----------------- %s", dot->getAesEncryption() ? TRUE : FALSE);
    INFO("Data Pending ------------------- %s", dot->getDataPending() ? TRUE : FALSE);
    INFO("ACK Requested ------------------ %s", dot->getAckRequested() ? TRUE : FALSE);
    INFO("Is Transmitting ---------------- %s", dot->getIsTransmitting() ? TRUE : FALSE);
    INFO("Idle Mode ---------------------- %s", dot->getIsIdle() ? TRUE : FALSE);

    INFO("Tx Data Rate ------------------- %u(%u %u)", dot->getTxDataRate(), dot->getMinDatarate(), dot->getMaxDatarate());
    INFO("Data Rate Offset --------------- (%u %u)", dot->getMinDatarateOffset(), dot->getMaxDatarateOffset());
    INFO("Rx2 Min Data Rate -------------- (%u %u)", dot->getMinRx2Datarate(), dot->getMaxRx2Datarate());
    INFO("Tx Power ----------------------- %lu(%u %u)", dot->getTxPower(), dot->getMinTxPower(), dot->getMaxTxPower());
    INFO("Antenna Gain ------------------- %i", dot->getAntennaGain());
    INFO("Tx Wait ------------------------ %s", dot->getTxWait() ? TRUE : FALSE);
    INFO("Min Frequency ------------------ %lu", dot->getMinFrequency());
    INFO("Max Frequency ------------------ %lu", dot->getMaxFrequency());
    INFO("Adaptive Data Rate ------------- %s", dot->getAdr() ? TRUE : FALSE);
    INFO("Forward Error Correction ------- %i", dot->getFec());
    INFO("CRC ---------------------------- %s", dot->getCrc() ? TRUE : FALSE);
    INFO("Repeat ------------------------- %u", dot->getRepeat());    
    
    INFO("Wake Pin ----------------------- %s", mDot::pinName2Str(dot->getWakePin()).c_str());
    INFO("LBT Time us -------------------- %u", dot->getLbtTimeUs());
    INFO("LBT Threshold ------------------ %i", dot->getLbtThreshold());

    INFO("ACK ---------------------------- %s", dot->getAck() ? TRUE : FALSE);
    INFO("Baud Rate ---------------------- %lu", dot->getBaud());
    
    INFO("Disable Duty Cycle ------------- %s", dot->getDisableDutyCycle() ? TRUE : FALSE);
    INFO("Echo --------------------------- %s", dot->getEcho() ? TRUE : FALSE);
    INFO("Flow Control ------------------- %s", dot->getFlowControl() ? TRUE : FALSE);
} 