#include "tonoka_mainFrame.h"

tonoka_mainFrame::tonoka_mainFrame( wxWindow* parent )
:
mainFrame( parent )
{
  #if defined(_WIN32)
  	SetMinSize(wxSize(850, 700));
  	SetSize(wxSize(850, 700));
  #elif defined(__WXMAC__)
  	SetMinSize(wxSize(850, 700));
  	SetSize(wxSize(850, 700));
  	m_about->Hide();
  	m_exit->Hide();
  #endif
  	// Create the UI
  	m_studies->InsertColumn(0, _(""));
	m_studies->InsertColumn(1, _("Name"));
	m_studies->InsertColumn(2, _("Date"));
	m_studies->InsertColumn(3, _("Description"));
	m_studies->InsertColumn(4, "StudyUID");
	m_studies->InsertColumn(5, "Path");
	m_studies->InsertColumn(6, "Status");
  	m_studies->EnableCheckboxes();

  	m_studies->SetColumnWidth(1, wxLIST_AUTOSIZE);
  	m_studies->SetColumnWidth(1, 200);
  	m_studies->SetColumnWidth(2, 100);
  	m_studies->SetColumnWidth(3, 200);
	m_studies->SetColumnWidth(4, 300);
	m_studies->SetColumnWidth(5, 300);

  	// Fill in settings
  	m_engine.LoadDestinationList();
  	m_engine.LoadGlobalDestinationList();

  	FillDestinationList();

  	wxConfig::Get()->SetPath("/Settings");
  	m_directory->SetValue(wxConfig::Get()->Read("Directory"));
  	m_destination->SetStringSelection(wxConfig::Get()->Read("Destination"));
	m_threads->SetValue(wxConfig::Get()->ReadLong("Threads", 4));
	m_depth->SetValue(wxConfig::Get()->ReadLong("Depth", 1));

	FillStudyList();
}

tonoka_mainFrame::~tonoka_mainFrame()
{
	wxConfig::Get()->SetPath("/Settings");
	wxConfig::Get()->Write("Directory", m_directory->GetValue());
	wxConfig::Get()->Write("Destination", m_destination->GetStringSelection());
	wxConfig::Get()->Write("Threads", m_threads->GetValue());
	wxConfig::Get()->Write("Depth", m_depth->GetValue());
	wxConfig::Get()->Flush();
}

void tonoka_mainFrame::OnBrowse( wxCommandEvent& event )
{
  wxDirDialog dlg(this, "", m_directory->GetValue(), wxRESIZE_BORDER | wxDD_DIR_MUST_EXIST);
	if(dlg.ShowModal() == wxID_OK)
	{
		m_directory->SetValue(dlg.GetPath());
	}
}

void tonoka_mainFrame::OnDestinationEdit( wxCommandEvent& event )
{
  tonoka_destination dlg(this);
	dlg.m_destinations = m_engine.destinations;

	if (dlg.ShowModal() == wxID_OK)
	{
		wxString oldsel1 = m_destination->GetStringSelection();
		m_engine.destinations = dlg.m_destinations;
		m_engine.SaveDestinationList();

		FillDestinationList();

		m_destination->SetStringSelection(oldsel1);
	}
}

void tonoka_mainFrame::m_studiesOnListColClick( wxListEvent& event )
{
// TODO: Implement m_studiesOnListColClick
}

void tonoka_mainFrame::OnClear( wxCommandEvent& event )
{
  m_studies->DeleteAllItems();
  m_engine.patientdata.Clear();
}

void tonoka_mainFrame::OnUpdate( wxCommandEvent& event )
{	
#ifdef _WIN32
	// on Windows, boost::filesystem::path is a wstring already
	boost::filesystem::path p = m_directory->GetValue();
#else
	boost::filesystem::path p = m_directory->GetValue().ToUTF8().data();
#endif
	m_engine.StartScan(p, m_depth->GetValue());


	tonoka_searchStatus dlg(this);
	dlg.m_scanner = &m_engine.scanner;

	// show and wait for thread to end.
	dlg.ShowModal();

	FillStudyList();
}

void tonoka_mainFrame::OnSend( wxCommandEvent& event )
{
	if (m_destination->GetSelection() == wxNOT_FOUND)
	{
		wxMessageBox(_("Please select a destination."), _("Error"), wxOK, this);
		return;
	}

	m_engine.StartSend(m_destination->GetSelection(), m_threads->GetValue());

	tonoka_sendStatus dlg(this);
	dlg.m_sender = &m_engine.sender;

	// show and wait for thread to end.
	dlg.ShowModal();

	FillStudyList();
}

void tonoka_mainFrame::OnAbout( wxCommandEvent& event )
{
  tonoka_about dlg(this);
  dlg.ShowModal();
}

void tonoka_mainFrame::OnExit( wxCommandEvent& event )
{
  Close();
}


void tonoka_mainFrame::FillDestinationList()
{
	// add to combo box
	m_destination->Clear();
	std::vector<DestinationEntry>::iterator itr;
	for(itr = m_engine.globalDestinations.begin(); itr != m_engine.globalDestinations.end(); itr++)
		m_destination->Append(wxString::FromUTF8((*itr).name.c_str()) + L" (*)");

	for(itr = m_engine.destinations.begin(); itr != m_engine.destinations.end(); itr++)
		m_destination->Append(wxString::FromUTF8((*itr).name.c_str()));
}

void tonoka_mainFrame::FillStudyList()
{
	m_studies->Disconnect(wxEVT_LIST_ITEM_CHECKED, wxListEventHandler(tonoka_mainFrame::m_studiesOnListItemChecked), NULL, this);
	m_studies->Disconnect(wxEVT_LIST_ITEM_UNCHECKED, wxListEventHandler(tonoka_mainFrame::m_studiesOnListItemUnchecked), NULL, this);

	m_studies->DeleteAllItems();
	m_engine.patientdata.GetStudies(boost::bind(&tonoka_mainFrame::fillstudiescallback, this, _1));

	m_studies->Connect(wxEVT_LIST_ITEM_CHECKED, wxListEventHandler(tonoka_mainFrame::m_studiesOnListItemChecked), NULL, this);
	m_studies->Connect(wxEVT_LIST_ITEM_UNCHECKED, wxListEventHandler(tonoka_mainFrame::m_studiesOnListItemUnchecked), NULL, this);
}

void tonoka_mainFrame::m_studiesOnListItemChecked(wxListEvent& event)
{
	m_engine.patientdata.SetStudyCheck(m_studies->GetItemText(event.GetIndex(), 4).ToUTF8().data(), true);
}

void tonoka_mainFrame::m_studiesOnListItemUnchecked(wxListEvent& event)
{
	m_engine.patientdata.SetStudyCheck(m_studies->GetItemText(event.GetIndex(), 4).ToUTF8().data(), false);
}


int tonoka_mainFrame::fillstudiescallback(Study &study)
{
	m_studies->InsertItem(0, "");
	m_studies->SetItem(0, 1, study.patname);

	struct tm thedate;
	memset(&thedate, 0, sizeof(struct tm));
	try
	{
		thedate.tm_year = boost::lexical_cast<int>(study.studydate.substr(0, 4)) - 1900;
		thedate.tm_mon = boost::lexical_cast<int>(study.studydate.substr(4, 2)) - 1;
		thedate.tm_mday = boost::lexical_cast<int>(study.studydate.substr(6, 2));
	}
	catch (...)
	{
		thedate.tm_year = 0;
		thedate.tm_mon = 0;
		thedate.tm_mday = 1;
	}

	char buf[1024];
	memset(buf, 0, 1024);

	strftime(buf, 1024, "%x", &thedate);

	m_studies->SetItem(0, 2, wxString::FromUTF8(buf));
	m_studies->SetItem(0, 3, wxString::FromUTF8(study.studydesc.c_str()));
	m_studies->SetItem(0, 4, wxString::FromUTF8(study.studyuid.c_str()));
	m_studies->SetItem(0, 5, study.path.c_str());
	m_studies->CheckItem(0, study.checked);
	return 0;
}