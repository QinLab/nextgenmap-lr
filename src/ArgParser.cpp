/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Contact: philipp.rescheneder@univie.ac.at
 */

#include "ArgParser.h"

#include <string.h>
#include <tclap/CmdLine.h>
#include <sstream>

#include "ArgParseOutput.h"
#include "Version.h"
#include "Log.h"
#include "PlatformSpecifics.h"

#undef module_name
#define module_name "ARGPARSER"

ArgParser::ArgParser(int argc, char * * argv) {

	outDefault = "stdout";
	noneDefault = "none";

	ParseArguments(argc, (char const * *) argv);
}

ArgParser::~ArgParser() {

}

char * ArgParser::fromString(std::string str) {
	char * cstr = 0;
	if(str.size() > 0 && str != outDefault && str != noneDefault) {
		cstr = new char[str.size() + 1];
		strcpy(cstr, str.c_str());
	}
	return cstr;
}

template<typename T>
void printParameter(std::stringstream & usage, TCLAP::ValueArg<T> & arg) {

	usage << "    " <<  arg.longID() << std::endl;
	usage << "        " << arg.getDescription();
	if(!arg.isRequired()) {
		usage << " [" << arg.getValue() << "]";
	}
	usage << std::endl;
}

void printParameter(std::stringstream & usage, TCLAP::SwitchArg & arg) {

	usage << "    " <<  arg.longID() << std::endl;
	usage << "        " << arg.getDescription();
	if(!arg.isRequired()) {
		usage << " [" << (arg.getValue() ? "true" : "false") << "]";
	}
	usage << std::endl;
}

void ArgParser::ParseArguments(int argc, char const * argv[]) {

	argv[0] = "ngmlr";

	TCLAP::CmdLine cmd("", ' ', "", true);

	TCLAP::ValueArg<std::string> queryArg("q", "query", "Path to the read file (FASTA/Q, SAM/BAM)", true, noneDefault, "file", cmd);
	TCLAP::ValueArg<std::string> refArg("r", "reference", "Path to the reference genome (FASTA/Q, can be gzipped)", true, noneDefault, "file", cmd);
	TCLAP::ValueArg<std::string> outArg("o", "output", "Path to output file", false, outDefault, "file", cmd);
	TCLAP::ValueArg<std::string> vcfArg("", "vcf", "SNPs will be taken into account when building reference index", false, noneDefault, "file", cmd);
	TCLAP::ValueArg<std::string> bedfilterArg("", "bed-filter", "Only reads in the regions specified by the BED file are read from the input file (requires BAM input)", false, noneDefault, "file", cmd);

	TCLAP::ValueArg<std::string> presetArgs("x", "presets", "Parameter presets for different sequencing technologies", false, "pacbio", "pacbio, ont", cmd);

	TCLAP::ValueArg<float> minIdentityArg("i", "min-identity", "Alignments with an identity lower than this threshold will be discarded", false, minIdentity, "0-1", cmd);
	TCLAP::ValueArg<float> minResiduesArg("R", "min-residues", "Alignments containing less than <int> or (<float> * read length) residues will be discarded", false, minResidues, "int/float", cmd);
	TCLAP::ValueArg<float> sensitivityArg("s", "sensitivity", "", false, sensitivity, "0-1", cmd);

	TCLAP::ValueArg<int> threadsArg("t", "threads", "Number of threads", false, 1, "int", cmd);

	TCLAP::ValueArg<int> binSizeArg("", "bin-size", "Sets the size of the grid used during candidate search", false, binSize, "int", cmd);
	TCLAP::ValueArg<int> kmerLengthArg("k", "kmer-length", "K-mer length in bases", false, kmerLength, "10-15", cmd);
	TCLAP::ValueArg<int> kmerSkipArg("", "kmer-skip", "Number of k-mers to skip when building the lookup table from the reference", false, kmerSkip, "int", cmd);
	TCLAP::ValueArg<float> scoreMatchArg("", "match", "Match score", false, scoreMatch, "float", cmd);
	TCLAP::ValueArg<float> scoreMismatchArg("", "mismatch", "Mismatch score", false, scoreMismatch, "float", cmd);
	TCLAP::ValueArg<float> scoreGapOpenArg("", "gap-open", "Gap open score", false, scoreGapOpen, "float", cmd);
	TCLAP::ValueArg<float> scoreGapExtendMaxArg("", "gap-extend-max", "Gap open extend max", false, scoreGapExtendMax, "float", cmd);
	TCLAP::ValueArg<float> scoreGapExtendMinArg("", "gap-extend-min", "Gap open extend min", false, scoreGapExtendMin, "float", cmd);
	TCLAP::ValueArg<int> stdoutArg("", "stdout", "Debug mode", false, stdoutMode, "0-7", cmd);
	TCLAP::ValueArg<int> readpartLengthArg("", "subread-length", "Length of fragments reads are split into", false, readPartLength, "int", cmd);
	TCLAP::ValueArg<int> readpartCorridorArg("", "subread-corridor", "Length of corridor sub-reads are aligned with", false, readPartCorridor, "int", cmd);
	//csSearchTableLength = 0;
	//logLevel = 0; //16383, 255
	//minKmerHits = 0;
	//maxCMRs = INT_MAX;

	TCLAP::SwitchArg noprogressArg("", "no-progress", "Don't print progress info while mapping", cmd, false);
	TCLAP::SwitchArg verboseArg("", "verbose", "Debug output", cmd, false);
	//bam = false;
	TCLAP::SwitchArg colorArg("", "color", "Colored command line output", cmd, false);
	//hardClip = false;
	//log = false;
	TCLAP::SwitchArg nolowqualitysplitArg("", "no-lowqualitysplit", "Don't split alignments with poor quality", cmd, false);
	TCLAP::SwitchArg nosmallInversionArg("", "no-smallinv", "Don't detect small inversions", cmd, false);
	TCLAP::SwitchArg printAllAlignmentsArg("", "print-all", "Print all alignments. Disable filtering. (debug)", cmd, false);
	//skipSave = false;
	//updateCheck = false;
	//writeUnmapped = true;
	TCLAP::SwitchArg fastArg("", "fast", "Debug switch (don't use if you don't know what you do)", cmd, false);

	std::stringstream usage;
	usage << "" << std::endl;
	usage << "Usage: ngmlr [options] -r <reference> -q <reads> [-o <output>]" << std::endl;
	usage << "" << std::endl;
	usage << "Input/Output:" << std::endl;
	printParameter<std::string>(usage, refArg);
	printParameter<std::string>(usage, queryArg);
	printParameter<std::string>(usage, outArg);
	usage << "" << std::endl;
	usage << "General:" << std::endl;
	printParameter<int>(usage, threadsArg);
	printParameter<std::string>(usage, presetArgs);
	printParameter<float>(usage, minIdentityArg);
	printParameter<float>(usage, minResiduesArg);
	printParameter(usage, nosmallInversionArg);
	printParameter(usage, nolowqualitysplitArg);
	printParameter(usage, verboseArg);
	printParameter(usage, noprogressArg);
	usage << "" << std::endl;
	usage << "Advanced:" << std::endl;
	printParameter<float>(usage, scoreMatchArg);
	printParameter<float>(usage, scoreMismatchArg);
	printParameter<float>(usage, scoreGapOpenArg);
	printParameter<float>(usage, scoreGapExtendMaxArg);
	printParameter<float>(usage, scoreGapExtendMinArg);
	printParameter<int>(usage, kmerLengthArg);
	printParameter<int>(usage, kmerSkipArg);
	printParameter<int>(usage, binSizeArg);
	printParameter<int>(usage, readpartLengthArg);
	printParameter<int>(usage, readpartCorridorArg);
	printParameter<std::string>(usage, vcfArg);
	printParameter<std::string>(usage, bedfilterArg);

	cmd.setOutput(new ArgParseOutput(usage.str(), ""));

	cmd.parse(argc, argv);

	queryFile = fromString(queryArg.getValue());
	referenceFile = fromString(refArg.getValue());
	outputFile = fromString(outArg.getValue());
	vcfFile = fromString(vcfArg.getValue());
	bedFile = fromString(bedfilterArg.getValue());

	minIdentity = minIdentityArg.getValue();
	minResidues = minResiduesArg.getValue();

	binSize = binSizeArg.getValue();
	kmerLength = kmerLengthArg.getValue();
	threads = threadsArg.getValue();
	kmerSkip = kmerSkipArg.getValue();
	scoreMatch = scoreMatchArg.getValue();
	scoreMismatch = scoreMismatchArg.getValue();
	scoreGapOpen = scoreGapOpenArg.getValue();
	scoreGapExtendMax = scoreGapExtendMaxArg.getValue();
	scoreGapExtendMin = scoreGapExtendMinArg.getValue();
	stdoutMode = stdoutArg.getValue();
	readPartCorridor = readpartCorridorArg.getValue();
	readPartLength = readpartLengthArg.getValue();

	progress = !noprogressArg.getValue();
	color = colorArg.getValue();
	verbose = verboseArg.getValue();
	lowQualitySplit = !nolowqualitysplitArg.getValue();
	smallInversionDetection = !nosmallInversionArg.getValue();
	printAllAlignments = printAllAlignmentsArg.getValue();
	fast = fastArg.getValue();

	if (presetArgs.getValue() == "pacbio") {
		//Do nothing. Defaults are for Pacbio
	} else if (presetArgs.getValue() == "ont") {
		lowQualitySplit = (nolowqualitysplitArg.isSet()) ? lowQualitySplit : true;
		scoreMatch = (scoreMatchArg.isSet()) ? scoreMatch : 1;
		scoreMismatch = (scoreMatchArg.isSet()) ? scoreMismatch : -1;
		scoreGapOpen = (scoreGapOpenArg.isSet()) ? scoreGapOpen : -1;
		scoreGapExtendMax = (scoreGapExtendMaxArg.isSet()) ? scoreGapExtendMax : -1;
		scoreGapExtendMax = (scoreGapExtendMinArg.isSet()) ? scoreGapExtendMin : -1;
	} else {
		std::cerr << "Preset " << presetArgs.getValue() << " not found" << std::endl;
	}

	std::stringstream fullCmd;
	fullCmd << std::string(argv[0]);
	for(int i = 1; i < argc; ++i) {
		fullCmd << " " << std::string(argv[i]);
	}
	fullCommandLineCall = fromString(fullCmd.str());

	if(!FileExists(queryFile)) {
		Log.Error("Query file (%s) does not exist.", queryFile);
	}
	if(!FileExists(referenceFile)) {
		Log.Error("Reference file (%s) does not exist.", referenceFile);
	}
	if(bedFile != 0 && !FileExists(bedFile)) {
		Log.Error("BED filter file (%s) does not exist.", bedFile);
	}
	if(vcfFile != 0 && !FileExists(vcfFile)) {
		Log.Error("SNP file (%s) does not exist.", vcfFile);
	}
}

