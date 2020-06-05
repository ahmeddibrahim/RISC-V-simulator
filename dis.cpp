#include <iostream>
#include  <iomanip>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(64*1024)
#define     CACHE_LINE_SIZE	 (32) // Size of the block in the cache
#define     NumberOfLines     (CACHE_SIZE/CACHE_LINE_SIZE) // Number of lines in the cache



struct cache // cache struct 
{
    int valid=0; // valid bit
    int tag; // tag
};
cache cache1[CACHE_SIZE/CACHE_LINE_SIZE]; // Array recognized as the cache 

enum cacheResType {MISS=0, HIT=1};

/* The following implements a random number generator */
unsigned int m_w = 0xABCCAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05786902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()// test case 1
{
    static unsigned int addr =1 ;
    return addr+1;
}

unsigned int memGen2()// test case 2
{
    static unsigned int addr=0;
    return addr%8;
}




unsigned int memGenA()
{
	static unsigned int addr=0;
	return (addr++)%(DRAM_SIZE/4);
}

unsigned int memGenB()
{
	static unsigned int addr=0;
	return  rand_()%(64*1024);
}

unsigned int memGenC()
{
	static unsigned int a1=0, a0=0;
	a0++;
	if(a0 == 512) { a1++; a0=0; }
	if(a1 == 128) a1=0;
	return(a1 + a0*128);
}

unsigned int memGenD()
{
	static unsigned int addr=0;
	return (addr++)%(1024*4);
}

unsigned int memGenE()
{
	static unsigned int addr=0;
	return (addr++)%(1024*16);
}

unsigned int memGenF()
{
	static unsigned int addr=0;
	return (addr+=64)%(128*1024);
}

// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr)
{	
    unsigned int offset= addr%CACHE_LINE_SIZE; 
    unsigned int Index = (addr/CACHE_LINE_SIZE)%NumberOfLines;
    unsigned int Tag   = (addr/(CACHE_LINE_SIZE*NumberOfLines));

    // check for miss or hit by either the tag or validaity 
    if((cache1[Index].tag!=Tag)||(cache1[Index].valid==0))
    {
        cache1[Index].tag=Tag;
        cache1[Index].valid=1;
        return MISS;
    }   
    else
        return HIT;
}


char *msg[2] = {"Miss","Hit"};

#define		NO_OF_Iterations	12	// CHange to 1,000,000
int main()
{
    unsigned int a[]={0,4,16,132,232,160,1024,30,140,3100,180,2180};// test case assignment
	float hit = 0, miss=0;
	cacheResType r;
	
	unsigned int addr;
	cout << "Direct Mapped Cache Simulator\n";

	for(int inst=0;inst<NO_OF_Iterations;inst++)
	{
		//addr = memGenD();
        addr= a[inst];
		r = cacheSimDM(addr);
		if(r == HIT) 
            hit++;
        else
            miss++;
		cout <<"0x" << setfill('0') << setw(8) << dec << addr <<" ("<< msg[r] <<")\n";
	}
	cout << "Hit ratio = "<<dec << (100*hit/NO_OF_Iterations)<< endl;
    cout << "Miss ratio = " <<dec<< (100*miss/NO_OF_Iterations)<< endl;
}