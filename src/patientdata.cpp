
#include "patientdata.h"
#include "sqlite3_exec_stmt.h"
#include <boost/locale.hpp>
#include <sstream>

PatientData::PatientData()
{
	db = NULL;
	createdb();
}

void PatientData::createdb()
{
	if (sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL) != SQLITE_OK)
	{
		std::ostringstream msg;
		msg << "Can't create database: " << sqlite3_errmsg(db);
		throw std::runtime_error(msg.str().c_str());				
	}	
		
	sqlite3_exec(db, "CREATE TABLE studies (studyuid TEXT UNIQUE, patid TEXT, patname TEXT, studydesc TEXT, studydate TEXT, path TEXT, checked TEXT)", NULL, NULL, NULL);	
}

PatientData::~PatientData()
{
	if(db)
		sqlite3_close(db);
}

void PatientData::Save()
{
	sqlite3 *backup;
	sqlite3_open_v2("tonoka.db", &backup, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL);	
	sqlite3_backup *bk = sqlite3_backup_init(backup, "main", db, "main");
	sqlite3_backup_step(bk, -1);
	sqlite3_backup_finish(bk);

	sqlite3_close(backup);
}

bool PatientData::Load()
{
	sqlite3 *backup = NULL;
	if (sqlite3_open_v2("tonoka.db", &backup, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL) != SQLITE_OK)
	{
		if (backup)		
			sqlite3_close(backup);
		return false;
	}

	sqlite3_backup *bk = sqlite3_backup_init(db, "main", backup, "main");
	sqlite3_backup_step(bk, -1);
	sqlite3_backup_finish(bk);

	sqlite3_close(backup);

	return true;
}

void PatientData::Clear()
{
	if(db)
		sqlite3_close(db);

	createdb();
}

int PatientData::AddStudy(std::string studyuid, std::string patid, std::string patname, std::string studydesc, std::string studydate, boost::filesystem::path path)
{
	sqlite3_stmt *insert;
	std::string sql = "INSERT INTO studies VALUES(?, ?, ?, ?, ?, ?, '1')";
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &insert, NULL);
	sqlite3_bind_text(insert, 1, studyuid.c_str(), studyuid.length(), SQLITE_STATIC);
	sqlite3_bind_text(insert, 2, patid.c_str(), patid.length(), SQLITE_STATIC);
	sqlite3_bind_text(insert, 3, patname.c_str(), patname.length(), SQLITE_STATIC);
	sqlite3_bind_text(insert, 4, studydesc.c_str(), studydesc.length(), SQLITE_STATIC);
	sqlite3_bind_text(insert, 5, studydate.c_str(), studydate.length(), SQLITE_STATIC);
#ifdef _WIN32
	std::string p = boost::locale::conv::utf_to_utf<char>(path.c_str());
#else
	std::string p = path.string();
#endif
	sqlite3_bind_text(insert, 6, p.c_str(), p.length(), SQLITE_STATIC);
	int res = sqlite3_exec_stmt(insert, NULL, NULL, NULL);
	sqlite3_finalize(insert);
	if(res == SQLITE_DONE || res == SQLITE_ABORT)
		return 1;
	else
		return 0;
}

int getstudiescallback(void *param,int columns,char** values, char**names)
{
	boost::function<int(Study &)> pfn = * static_cast<boost::function<int(Study &)> *>(param);
#ifdef _WIN32		
	Study result(values[0], values[1], values[2], values[3], values[4], boost::locale::conv::utf_to_utf<boost::filesystem::path::value_type>(values[5]), values[6][0] == '1');
#else
	Study result(values[0], values[1], values[2], values[3], values[4], values[5], values[6][0] == '1');
#endif	
	return pfn(result);
}

// void PatientData::GetStudies(std::vector<Study> &studies)
void PatientData::GetStudies(boost::function< int(Study &) > action)
{
	std::string selectsql = "SELECT studyuid, patid, patname, studydesc, studydate, path, checked FROM studies ORDER BY studyuid ASC";
	sqlite3_stmt *select;
	sqlite3_prepare_v2(db, selectsql.c_str(), selectsql.length(), &select, NULL);	
	sqlite3_exec_stmt(select, getstudiescallback, &action, NULL);		
	sqlite3_finalize(select);
}

void PatientData::GetStudies(std::string patientid, std::string patientname, boost::function< int(Study &) > action)
{
	std::string selectsql = "SELECT studyuid, patid, patname, studydesc, studydate, path, checked FROM studies WHERE (patid = ? AND patname = ?) ORDER BY studyuid ASC";
	sqlite3_stmt *select;
	sqlite3_prepare_v2(db, selectsql.c_str(), selectsql.length(), &select, NULL);
	sqlite3_bind_text(select, 1, patientid.c_str(), patientid.length(), SQLITE_STATIC);
	sqlite3_bind_text(select, 2, patientname.c_str(), patientname.length(), SQLITE_STATIC);
	sqlite3_exec_stmt(select, getstudiescallback, &action, NULL);		
	sqlite3_finalize(select);
}

void PatientData::SetStudyCheck(std::string studyuid, bool checked)
{
	std::string selectsql = "UPDATE studies SET checked = ? WHERE (studyuid = ?)";
	sqlite3_stmt *select;
	std::string checkstr = "1";
	if (!checked)
		checkstr = "0";
	sqlite3_prepare_v2(db, selectsql.c_str(), selectsql.length(), &select, NULL);
	sqlite3_bind_text(select, 1, checkstr.c_str(), checkstr.length(), SQLITE_STATIC);
	sqlite3_bind_text(select, 2, studyuid.c_str(), studyuid.length(), SQLITE_STATIC);	
	sqlite3_exec_stmt(select, getstudiescallback, NULL, NULL);
	sqlite3_finalize(select);
}
