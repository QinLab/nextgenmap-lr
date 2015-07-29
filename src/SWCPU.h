/*
 * SWCPU.h
 *
 *  Created on: Jun 15, 2011
 *      Author: fritz
 */

#ifndef SWCPU_H_
#define SWCPU_H_
/*
 * SWCuda.h
 *
 *  Created on: Jan 10, 2011
 *      Author: fritz
 */

#define pRef pBuffer1
#define pQry pBuffer2

#include "IAlignment.h"
#include "ILog.h"
#include "IConfig.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using std::endl;
using std::cout;
using std::max;

#define CIGAR_STOP 10
#define short_min -16000
#define result_number 4
#define line_end '\0'
#define ref_position 0
#define qstart 1
#define qend 2
#define alignment_offset 3
#define param_best_read_index 0
#define param_best_ref_index 1

#define CIGAR_M 0
#define CIGAR_I 1
#define CIGAR_D 2
#define CIGAR_N 3
#define CIGAR_S 4
#define CIGAR_H 5
#define CIGAR_P 6
#define CIGAR_EQ 7
#define CIGAR_X 8


struct MatrixElement {
	short score;
	short indelRun;
	char direction;
};


const char trans[256] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0,
		4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4,
		4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

class SWCPUCor: public IAlignment {
public:
	SWCPUCor(int gpu_id);
	virtual ~SWCPUCor();
	virtual int GetScoreBatchSize() const;
	virtual int GetAlignBatchSize() const;
	virtual int BatchScore(int const mode, int const batchSize,
			char const * const * const refSeqList,
			char const * const * const qrySeqList, float * const results,
			void * extData);
	virtual int BatchAlign(int const mode, int const batchSize,
			char const * const * const refSeqList,
			char const * const * const qrySeqList, Align * const results,
			void * extData);

	virtual int SingleAlign(int const mode, int const corridor,
			char const * const refSeq, char const * const qrySeq,
			Align & result, void * extData);
private:

	bool cigar;
	short scores[6][6];
	short mat;
	short mis;
	short gap_read;
	short gap_ref;
	//meta info
	unsigned int batch_size; //effictive thread number that is started per call

	int printCigarElement(char const op, short const length, char * cigar);

	void computeCigarMD(Align & result, int const gpuCigarOffset,
			short const * const gpuCigar, char const * const refSeq, int corr_length, int read_length);

	float SW_Score(char const * const scaff, char const * const read, short * result, int corr_length, MatrixElement * mat_pointer, short * local_mat_line);

	void Backtracking_CIGAR(char const * const scaff, char const * const read,
			short *& result, short *& alignments, int corr_length, int read_length, int alignment_length, MatrixElement * mat_pointer);

	void print_matrix(int alignment_length, const char* const refSeq,
			int read_length, const char* const qrySeq, int corr_length,
			MatrixElement* mat_pointer);
};

#endif /* SWCPU_H_ */