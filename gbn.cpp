#include "../include/simulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <queue>
#include <vector>

using namespace std;

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
int seqnumA, acknumA,seqnumnext;


struct pkt currentpacket;

int winsize, winbeginning; //  
static const float RTT = 16.0; 

vector <msg> buffer;

bool IsCorrupted(struct pkt packet);
int calc_checksum(pkt packet);
void udt_send();
void messagetopacket(msg message, int seqnum, int acknum);
void makeack(int acknum);



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

void messagetopacket(msg message, int seqnum, int acknum)
{

    pkt currentpacket = {};
    strncpy(currentpacket.payload, message.data, sizeof(currentpacket.payload));
    currentpacket.seqnum = seqnum;
    currentpacket.acknum = acknum;
    currentpacket.checksum = calc_checksum(currentpacket);
    tolayer3(0,currentpacket);

}

void makeack(int acknum)
{
    pkt ackpacket = {};
    ackpacket.acknum = acknum;
    ackpacket.checksum = calc_checksum(ackpacket);
    tolayer3(1,ackpacket);
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    //adding message to buffer
    buffer.push_back(message);
    //sending the data to lower layer
    udt_send();

}

void udt_send()
{
    while((seqnumnext < buffer.size()) && (seqnumnext < winbeginning + winsize))
    {
        messagetopacket(buffer[seqnumnext],seqnumnext,acknumA);

        if(winbeginning == seqnumnext)
            starttimer(0,RTT);
    seqnumnext++;    
    }
    
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    if(!IsCorrupted(packet))
    {
        winbeginning = packet.acknum + 1;

        if(winbeginning == seqnumnext)
            stoptimer(0);
        else
        {
            stoptimer(0);
            starttimer(0, RTT);
        }
    }

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    seqnumnext = winbeginning;
    udt_send();
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    seqnumA = 0;
    acknumA = 0;
    winsize = getwinsize();
    winbeginning = 0;
    seqnumnext = 0;

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

int seqnumB,seqnumexpected;
/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    if(!IsCorrupted(packet) && (seqnumexpected == packet.seqnum))
    {
        tolayer5(1,packet.payload);
        makeack(seqnumexpected);

        seqnumexpected++;

    }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    seqnumexpected = 0;
}
