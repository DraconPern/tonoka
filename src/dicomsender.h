#ifndef _DICOMSENDER_
#define _DICOMSENDER_

#include "alphanum.hpp"
#include <set>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio/io_service.hpp>
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

	void SendStudy(boost::filesystem::path path);

	typedef std::map<std::string, std::set<std::string> > mapset;
	typedef std::map<std::string, boost::filesystem::path, doj::alphanum_less<std::string> > naturalpathmap;
	int SendABatch(const mapset &sopclassuidtransfersyntax, naturalpathmap &instances);

	void ScanDir(boost::filesystem::path path, naturalpathmap &instances, mapset &sopclassuidtransfersyntax, std::string &study_uid);
	void ScanFile(boost::filesystem::path path, naturalpathmap &instances, mapset &sopclassuidtransfersyntax, std::string &study_uid);

	bool IsCanceled();
	void ClearCancel();
	void SetDone(bool state);

	// threading data
	boost::mutex mutex;
	bool cancelEvent, doneEvent;	
	DestinationEntry m_destination;
	int m_threads;	
	
	int fillstudies(Study &study);

	boost::asio::io_service service;
	
	std::vector<boost::filesystem::path> study_dirs;	// list of directories that we are sending	
};


#endif