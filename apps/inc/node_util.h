#include "mDot.h"

using namespace std;
extern mDot* dot;

void setPower(uint8_t& power);
void setDataRate(uint8_t& datarate);
void setPacketLength(vector<uint8_t>& data, uint8_t& len);
bool isSending();
uint8_t send(vector<uint8_t>& data);
void linkCheck();
void displayInformation();