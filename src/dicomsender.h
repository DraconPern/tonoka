#ifndef _DICOMSENDER_
#define _DICOMSENDER_

#include "alphanum.hpp"
#include <set>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include "patientdata.h"
#include "destinationentry.h"

class DICOMSender
{

public:
	DICOMSender(PatientData &patientdata);
	~DICOMSender(void);

	void DoSendAsync(DestinationEntry destination, int threads);
	void DoSend(DestinationEntry destination, int threads);

	static bool Echo(DestinationEntry destination);

	void Cancel();
	bool IsDone();
protected:
	static void DoSendThread(void *obj);
	PatientData &patientdata;

	int SendABatch();

	bool IsCanceled();
	void ClearCancel();
	void SetDone(bool state);

	// threading data
	boost::mutex mutex;
	bool cancelEvent, doneEvent;	
	DestinationEntry m_destination;
	int m_threads;

	typedef std::map<std::string, std::set<std::string> > mapset;
	mapset sopclassuidtransfersyntax;

	typedef std::map<std::string, boost::filesystem::path, doj::alphanum_less<std::string> > naturalpathmap;
	int fillstudies(Study &study);
	
	std::vector<std::string> studies;
	naturalpathmap instances;	// sopid, filename, this ensures we send out instances in sopid order	
};


#endif