#include "wx/wx.h"
#include "wx/app.h"
#include <wx/log.h>
#include "wx/config.h"
#include "wx/display.h"
#include <wx/dir.h>
#include "wx/toolbook.h"
#include "wx/aui/auibook.h"
#include "wx/aui/auibar.h"
#include "wx/infobar.h"
#include "wx/stdpaths.h"
#include "wx/aui/framemanager.h"
#include "wx/menu.h"
#include "wx/platinfo.h"

#include "wx/webview.h"
#if wxUSE_WEBVIEW_IE
#include "wx/msw/webview_ie.h"
#endif
#if wxUSE_WEBVIEW_EDGE
#include "wx/msw/webview_edge.h"
#endif
#include "wx/webviewarchivehandler.h"
#include "wx/webviewfshandler.h"

#include <filesystem>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>

#include <stdio.h>
#include <string.h>
#include "wx/tokenzr.h"
#include "wx/event.h"

#include <nlohmann/json.hpp>




class SimulHelper final
{
    public:
        enum ScriptResult
        {
            HelloConsoleLog
        };

        SimulHelper();

        void SetWebView(wxWebView* webView);

        void RunHello(std::string hellostring);

    private:
        wxWebView* m_webView{nullptr};
        bool is_simul_webview_set = false;
};


using json = nlohmann::ordered_json;


SimulHelper::SimulHelper()
{}

void SimulHelper::RunHello(std::string hellostring)
{
    wxCHECK_RET(m_webView, "m_webView is null");
    wxString script;

    try
    {
        json j;
        j["Hello"] = hellostring;
        std::string jd = j.dump();
        const char* jd_c = jd.c_str();
        wxLogDebug(jd_c);
        script.Printf("HelloConsoleLog('%s');", j.dump());
        wxLogDebug("script: ", script);
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON error in %s (%s)."), __FUNCTION__, e.what());
    }

    m_webView->RunScriptAsync(script, (void*)HelloConsoleLog);
}

void SimulHelper::SetWebView(wxWebView* webView)
{
    wxASSERT(webView);
    m_webView = webView;
    is_simul_webview_set = true;
}

namespace MyNs {

    using json = nlohmann::json;

    class Frame : public wxFrame {

        public:

           Frame(wxWindow* parent, const wxString _simulAssetsFolder) : wxFrame(nullptr, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {

                wxDisplay* display = new wxDisplay();

                wxRect clientArea = display->GetClientArea();
                wxSize clientAreaSize = clientArea.GetSize();
                int clientAreaHeight = clientAreaSize.GetHeight();
                int clientAreaWidth = clientAreaSize.GetWidth();

                tabControl1->AddPage(tabPageH, "H", true);

                simulAssetsFolder = _simulAssetsFolder;

                std::wstring currentWd (currentWd_s.begin(), currentWd_s.end());
                HtmlPath = currentWd + sep + L"/src/simul-assets/simul.html";
                std::string HtmlPath_s (HtmlPath.begin(), HtmlPath.end());

                #if defined(unix) || defined(__unix__) || defined(__unix)
                # define PREDEF_PLATFORM_UNIX
                HtmlPathOS = "file://" + HtmlPath_s;
                #endif

                CreateSimulPage(tabPageH);

                Bind(wxEVT_WEBVIEW_CREATED, [this](wxWebViewEvent&) { SimulConfigureWebView(); });
                Bind(wxEVT_WEBVIEW_LOADED, &Frame::SimulOnWebViewLoaded, this);
                //Bind(wxEVT_WEBVIEW_ERROR, &Frame::OnError, this, m_browser->GetId());
                Bind(wxEVT_WEBVIEW_SCRIPT_RESULT, &Frame::OnScriptResult, this, m_browser->GetId());
            }

           private:
                wxString os_family = wxPlatformInfo::Get().GetOperatingSystemFamilyName();
                std::wstring sep = (os_family == "Windows") ? L"\\" : L"/";

                std::string currentWd_s = std::filesystem::current_path();

                wxInfoBar *m_info;

                wxSize sizeTabControl1 = wxGetDisplaySize();

                std::wstring HtmlPath;
                std::string HtmlPathOS;

                wxPanel* mainPanel = new wxPanel(this);
                wxToolbook* tabControl1 = new wxToolbook(mainPanel, wxID_ANY, wxDefaultPosition, sizeTabControl1, /*wxTBK_BUTTONBAR|*/wxTBK_HORZ_LAYOUT, "");
                wxNotebookPage* tabPageH = new wxNotebookPage(tabControl1, wxID_ANY);

                wxSize sizeWeb = wxGetDisplaySize();
                wxString simulAssetsFolder;
                SimulHelper m_simulHelper;
                wxWebView* m_browser;
                wxString m_browser_backend;
                bool m_browser_configured = false;
                void OnError(wxWebViewEvent& evt);
                void SimulConfigureWebView();
                void SimulOnWebViewLoaded(wxWebViewEvent&);
                void SimulOnMessageError(const wxArrayString& params, const wxString& msg);
                void SimulOnWebViewMessageReceived(wxWebViewEvent& evt);
                void RunHello();
                wxPanel* CreateSimulPage(wxNotebookPage* parent);
                void OnScriptResult(wxWebViewEvent& evt);
    };


    void Frame::SimulConfigureWebView()
    {
        if ( m_browser )
            return;

        void* nativeBackend = m_browser->GetNativeBackend();
        if ( !nativeBackend )
            return;

        m_browser_configured = true;
    }

    void Frame::SimulOnWebViewLoaded(wxWebViewEvent&)
    {
        RunHello();
    }

    void Frame::SimulOnMessageError(const wxArrayString& params, const wxString& msg)
    {
        constexpr size_t validMinParamsCount = 2;
        if (params.size() < validMinParamsCount)
        {
            wxLogError(_("Malformed simul error message: '%s'"),msg);
            return;
        }
    }

    void Frame::SimulOnWebViewMessageReceived(wxWebViewEvent& evt)
    {
        wxString msg;

        // all the simul messages start with "simul::" and after that, the
        // indivdual message fields are separated by \t, the first one being
        // the message type and the rest message-type specific
        if (evt.GetString().StartsWith("simul::", &msg))
        {
            constexpr char msgFieldDelimiter = '\t';
            wxArrayString msgFields = wxStringTokenize(msg, msgFieldDelimiter);
            if (msgFields.size() < 2)
            {
                wxLogError(_("Invalid simul message: '%s'."), msg);
                return;
            }
            const wxString msgType = msgFields[0];
            msgFields.erase(msgFields.begin()); // remove the message type item
            if (msgType == "error")
            {
                SimulOnMessageError(msgFields, msg);
            }
            else
            {
                wxLogMessage(_("Unknown simul message type '%s' ('%s')."), msgType, msg);
            }
        }
    }

    void Frame::RunHello()
    {
        m_simulHelper.SetWebView(m_browser);
        m_simulHelper.RunHello("HALOOOOOOOOO");
    }

    wxPanel* Frame::CreateSimulPage(wxNotebookPage* parent)
    {
        wxPanel* simulPanel = new wxPanel(parent);
        wxBoxSizer* topSimulSizer = new wxBoxSizer(wxVERTICAL);
        topSimulSizer->AddSpacer(100);
        topSimulSizer->Add(simulPanel, wxSizerFlags().Expand());
        wxBoxSizer* simul_panel_sizer = new wxBoxSizer(wxVERTICAL);
        simulPanel->SetSizer(simul_panel_sizer);

        m_info = new wxInfoBar(parent);
        topSimulSizer->Add(m_info, wxSizerFlags().Expand());

        #if wxUSE_WEBVIEW_EDGE
        // Check if a fixed version of edge is present in $executable_path/edge_fixed and use it
        wxFileName edgeFixedDir(wxStandardPaths::Get().GetExecutablePath());
        edgeFixedDir.SetFullName("");
        edgeFixedDir.AppendDir("edge_fixed");
        if (edgeFixedDir.DirExists())
        {
            wxWebViewEdge::MSWSetBrowserExecutableDir(edgeFixedDir.GetFullPath());
            wxLogMessage("Using fixed edge version");
        }
        #endif

        m_browser = wxWebView::New();

        // With several backends the proxy can only be set before creation, so do it here if the standard environment variable is defined.
        wxString proxy;
        if ( wxGetEnv("http_proxy", &proxy) )
        {
            if ( m_browser->SetProxy(proxy) )
                wxLogMessage("Using proxy \"%s\"", proxy);
            else {
                wxLogMessage("error message should have been already given by wxWebView itself");
            }
        }
        wxSize simulWindowSize = wxGetDisplaySize();

        wxWindowDC simulDCWindow(this); // Create a DC for the whole screen area
        // Get the size of the windownDC
        wxCoord windowWidth, windowHeight;
        simulDCWindow.GetSize(&windowWidth, &windowHeight);
        wxSize simul_bs(windowWidth * 0.98, windowHeight * 0.98);
        m_browser->Create(simulPanel, wxID_ANY, HtmlPathOS, wxDefaultPosition, simul_bs);
        topSimulSizer->Add(m_browser, wxSizerFlags().Expand());
        m_browser->SetFocus();

        //new wxLogWindow(scenario_browser_tab, ("ScenarioBrowserTabLogging"), true, false);

        if (m_browser->AddScriptMessageHandler("wxmsg"))
        {
            m_browser->Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &Frame::SimulOnWebViewMessageReceived, this);
        }
        m_browser->ShowDevTools();
        //ScenarioSimulationOnShowDevTools();
        SimulConfigureWebView();

        return simulPanel;
    }

    void Frame::OnScriptResult(wxWebViewEvent& evt)
    {
        if (evt.IsError())
            wxLogError("Async script execution failed: %s", evt.GetString());
        else
            wxLogMessage("Async script result received; value = %s", evt.GetString());
    }

    //void Frame::OnError(wxWebViewEvent& evt)
    //{
    //#define WX_ERROR_CASE(type) \
        //case type: \
            //category = #type; \
            //break;

        //wxString category;
        //switch (evt.GetInt())
        //{
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CONNECTION);
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CERTIFICATE);
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_AUTH);
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_SECURITY);
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_NOT_FOUND);
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_REQUEST);
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_USER_CANCELLED);
            //WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_OTHER);
        //}
        //wxLogMessage("Error; url='%s', error='%s (%s)'",
                 //evt.GetURL(), category, evt.GetString());
        //Show the info bar with an error
        //m_info->ShowMessage(_("An error occurred loading ") + evt.GetURL() + "\n" +
        //"'" + category + "'", wxICON_ERROR);
    //}


    class Application : public wxApp {

        bool OnInit() override {

        #if USING_WEBVIEW_EDGE
            if ( !wxWebView::IsBackendAvailable(wxWebViewBackendEdge) )
            {
                wxLogError(_("Cannot use wxWebViewEdge backend: Is 'WebView2Loader.dll' in the same folder as the executable?"));
                return false;
            }
        #endif // #if USING_WEBVIEW_EDGE

            SetAppName("MyApp");

        #ifdef __WXMSW__
            wxStandardPaths& standardPaths = wxStandardPaths::Get();

            standardPaths.IgnoreAppSubDir("Debug DLL");
            standardPaths.IgnoreAppSubDir("Release DLL");
            standardPaths.IgnoreAppSubDir("x64");
        #endif

            delete wxConfigBase::Set(new wxConfig(GetAppName(), GetVendorName()));

            const wxString simulAssetsFolder = GetSimulAssetsFolder();

            if ( simulAssetsFolder.empty() )
            {
                wxLogError(_("Could not establish the notes assets folder: The application will terminate."));
                return false;
            }

            (new Frame(nullptr, simulAssetsFolder))->Show();

            return true;
        }

        wxString GetSimulAssetsFolder()
        {
            static constexpr const char* Assets[] = {"simul.html"};
            static constexpr auto assetsFolderName = "simul-assets";
            static constexpr auto configKeyName = "SimulAssetsFolder";

            const wxStandardPaths& standardPaths = wxStandardPaths::Get();
            wxConfigBase* config = wxConfigBase::Get();
            const wxConfigPathChanger changer(config, "/");
            wxString folder;
            wxFileName fn;

            auto HasAssets = [](const wxString& folder)
            {
                for ( const auto name : Assets )
                {
                    if ( !wxFileName(folder, name).FileExists() )
                    {
                        return false;
                    }
                }
                return true;
            };
            // first try the path stored in the config, if any
            if ( config->Read(configKeyName, &folder) )
            {
                if ( HasAssets(folder) )
                {
                    return folder;
                }
            }
            // the standard data dir
            fn.AssignDir(standardPaths.GetDataDir());
            fn.AppendDir(assetsFolderName);
            folder = fn.GetPath();
            if ( HasAssets(folder) )
            {
                return folder;
            }
            // the folder with the executable
            fn.Assign(standardPaths.GetExecutablePath());
            fn.AppendDir(assetsFolderName);
            folder = fn.GetPath();
            if ( HasAssets(folder) )
            {
                return folder;
            }
            // try the folder above the one with the executable
            if ( fn.GetDirCount() > 2 )
            {
                fn.RemoveDir(fn.GetDirCount() - 2);
                folder = fn.GetPath();
                if ( HasAssets(folder) )
                {
                    return folder;
                }
            }
            // could not find, ask the user
            for ( ;; )
            {
                wxFileDialog dlg(nullptr, _("Select the folder with siimul assets"), "", Assets[0],
                         _("HTML Files (*.html)|*.html"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

                if ( dlg.ShowModal() != wxID_OK )
                {
                    break;
                }

                fn.Assign(dlg.GetPath());
                folder = fn.GetPath();
                if ( HasAssets(folder) )
                {
                    config->Write(configKeyName, folder);
                    return folder;
                }
            }
            // assets folder undetermined
            return wxEmptyString;
        };


        int onExit()
        {
            delete wxConfigBase::Set(nullptr);
            return wxApp::OnExit();
        }

    }; // end of class Application


} // end of namespace


wxIMPLEMENT_APP(MyNs::Application);
