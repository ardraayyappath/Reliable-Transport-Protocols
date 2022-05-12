

#include "../include/simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <queue>

using namespace std;



bool ackreceived;
int seqnumA,acknumA;

struct pkt currentpacket;


static const int winsize = 1; // for alternating bit, window size is 1
static const float RTT = 14.0; // should change and check

queue<msg> buffer;

bool IsCorrupted(struct pkt packet);
int calc_checksum(pkt packet);


bool IsCorrupted(struct pkt packet)
{


    if(packet.checksum == calc_checksum(packet))
        return false;
    else
        return true;
}


int calc_checksum(struct pkt packet)
{
//computes checksum
int checksum = packet.seqnum + packet.acknum;
for(int i =0; i<20;i++)
{
  checksum+=packet.payload[i];
}
return checksum;
}


/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{

	
    if(ackreceived)
    {
        ackreceived = false; //resetting the value of variable to wait for the next ACK
        currentpacket = {}; //resetting the currentpacket to add data

        strncpy(currentpacket.payload, message.data, sizeof(currentpacket.payload));
        currentpacket.seqnum = seqnumA;
        currentpacket.acknum = acknumA;
        currentpacket.checksum = calc_checksum(currentpacket);
        tolayer3(0,currentpacket);
        starttimer(0,RTT);

    }
    else
    {
        buffer.push(message);
    }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	if(!IsCorrupted(packet) && packet.acknum == seqnumA)
	{ // packet is not corrupt and B is ready to get next seq number
		
		
		seqnumA = !seqnumA; //toggling between the two sequence numbers in ABT
		stoptimer(0);
		if(!buffer.empty())
        {
			msg next_message = buffer.front();
			buffer.pop();
			ackreceived = true;
			A_output(next_message);	
		}
		else
			ackreceived = true;	
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    ackreceived = false;
    tolayer3(0,currentpacket);
    starttimer(0,RTT);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    ackreceived = true;
    seqnumA = 0;
    acknumA = 0;

}
int seqnumB, acknumB;
/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    pkt currentpacket = {};

    if(!IsCorrupted(packet) && (packet.seqnum == seqnumB))
    {
        tolayer5(1,packet.payload);
        currentpacket.acknum = seqnumB;
        currentpacket.checksum = calc_checksum(currentpacket);
        tolayer3(1,currentpacket);
        seqnumB = !seqnumB;

    }

    else if (!IsCorrupted(packet) && packet.seqnum != seqnumB)
    {
        currentpacket.acknum = !seqnumB;
        currentpacket.checksum = calc_checksum(packet);
        tolayer3(1,currentpacket);
    }


}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	seqnumB=0;
}
