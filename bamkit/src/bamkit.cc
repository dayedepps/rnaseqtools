#include <cstdio>
#include <cassert>
#include <sstream>

#include "config.h"
#include "bamkit.h"

bamkit::bamkit(const string &bamfile)
{
    sfn = sam_open(bamfile.c_str(), "r");
    hdr = sam_hdr_read(sfn);
    b1t = bam_init1();
	qlen = 0;
	qcnt = 0;
	plen = 0;
	pcnt = 0;
}

bamkit::~bamkit()
{
    bam_destroy1(b1t);
    bam_hdr_destroy(hdr);
    sam_close(sfn);
}

int bamkit::solve()
{
    while(sam_read1(sfn, hdr, b1t) >= 0)
	{
		bam1_core_t &p = b1t->core;

		if((p.flag & 0x4) >= 1) continue;										// read is not mapped
		if((p.flag & 0x100) >= 1 && use_second_alignment == false) continue;	// secondary alignment
		if(p.n_cigar > MAX_NUM_CIGAR) continue;									// ignore hits with more than 7 cigar types
		if(p.qual < min_mapping_quality) continue;								// ignore hits with small quality
		if(p.n_cigar < 1) continue;												// should never happen

		hit ht(b1t);

		qlen += ht.qlen;
		qcnt += 1;

		if(ht.isize <= 0) continue;

		plen += ht.isize;
		pcnt += 1;
	}

	printf("aligned reads = %d aligned base pair = %.0lf alignment segments = %d total insert size = %.0lf\n", qcnt, qlen, pcnt, plen);

	return 0;
}