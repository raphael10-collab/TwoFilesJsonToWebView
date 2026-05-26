#include <sqlite3.h>

#include "wx/wx.h"
#include "wx/app.h"
#include <wx/sckipc.h>
#include <wx/log.h>
#include <wx/thread.h>
#include <wx/checkbox.h>
#include "wx/radiobox.h"
#include "wx/statbox.h"
#include "wx/config.h"
#include "wx/filedlg.h"
#include "wx/display.h"
#include <wx/dir.h>
#include <wx/msgqueue.h>
#include "wx/dataview.h"
#include <wx/hyperlink.h>
#include "wx/datetime.h"
#include "time.h"
#include <ctime>
#include "wx/scrolwin.h"
#include "wx/glcanvas.h"
#include <wx/persist/toplevel.h>
#include "wx/tipwin.h"
#include "wx/tipdlg.h"
#include "wx/accel.h"
#include "wx/popupwin.h"
#include "wx/toolbook.h"
#include "wx/aui/auibook.h"
#include "wx/aui/auibar.h"
#include "wx/infobar.h"
#include "wx/filesys.h"
#include "wx/listctrl.h"
#include "wx/fs_arc.h"
#include "wx/fs_mem.h"
#include "wx/stdpaths.h"
#include <wx/filename.h>
#include "wx/colordlg.h"
#include "wx/dcclient.h"
#include "wx/aui/framemanager.h"
#include "wx/menu.h"
#include "wx/mimetype.h"
#include "wx/filefn.h"
#include "wx/artprov.h"
#include "wx/cmdline.h"
#include "wx/notifmsg.h"
#include "wx/settings.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "wx/richtext/richtextformatdlg.h"
#include "wx/richtext/richtextsymboldlg.h"
#include "wx/richtext/richtextstyledlg.h"
#include "wx/richtext/richtextimagedlg.h"
#include "wx/richtext/richtextprint.h"
#include "wx/richtext/richtextbuffer.h"
#include "wx/mousestate.h"
#include "wx/radiobut.h"
#include "wx/image.h"
#include "wx/artprov.h"
#include "wx/dcbuffer.h"
#include "wx/dcgraph.h"
#include "wx/overlay.h"
#include "wx/graphics.h"
#include "wx/filename.h"
#include "wx/metafile.h"
#include "wx/settings.h"
#include "wx/gdicmn.h"
#include "wx/platinfo.h"

#if wxUSE_SVG
#include "wx/dcsvg.h"
#endif
#if wxUSE_POSTSCRIPT
#include "wx/dcps.h"
#endif
#include "wx/webview.h"
#if wxUSE_WEBVIEW_IE
#include "wx/msw/webview_ie.h"
#endif
#if wxUSE_WEBVIEW_EDGE
#include "wx/msw/webview_edge.h"
#endif
#include "wx/webviewarchivehandler.h"
#include "wx/webviewfshandler.h"

#if wxUSE_STC
#include "wx/stc/stc.h"
#else
#include "wx/textctrl.h"
#endif

#include <sys/stat.h>
#include <filesystem>
#include <cstdlib>
#include <clocale>
#include <cstdio>
#include <iostream>
#include <format>
#include <text_encoding>
#include <cstring>
#include <string>
#include <charconv>
#include <sstream>
#include <map>
#include <algorithm>
#include <cctype>

#include <thread>
#include <future>
#include "chrono"

#include <typeinfo>
#include <cxxabi.h>
#include <fstream>
#include <cstdlib> // for exit function
#include <utility>
#include <memory>
#include <type_traits>
#include "wx/dcmirror.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <curl/curl.h>
#include <math.h>
#include <ranges>
#include <wx/grid.h>
#include <wx/headerctrl.h>
#include <wx/generic/gridctrl.h>
#include <wx/generic/grideditors.h>
#include "wx/splitter.h"
#include "wx/base64.h"
#include "wx/ffile.h"
#include "wx/mstream.h"
#include "wx/numdlg.h"
#include "wx/statline.h"
#include "wx/stdpaths.h"
#include "wx/tokenzr.h"
#include "wx/event.h"
#include <signal.h>
#include <inttypes.h>


#include "webkit2/webkit2.h"

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

            }

           private:
                wxString os_family = wxPlatformInfo::Get().GetOperatingSystemFamilyName();
                std::wstring sep = (os_family == "Windows") ? L"\\" : L"/";

                std::string currentWd_s = std::filesystem::current_path();

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
                void SimulConfigureWebView();
                void SimulOnShowDevTools();
                void SimulOnMessageError(const wxArrayString& params, const wxString& msg);
                void SimulOnWebViewMessageReceived(wxWebViewEvent& evt);
                void RunHello();
                wxPanel* CreateSimulPage(wxNotebookPage* parent);
                void SendStringToJS( const wxString &payload );
    };


    void Frame::SimulConfigureWebView()
    {
        if ( m_browser )
            return;

        void* nativeBackend = m_browser->GetNativeBackend();
        if ( !nativeBackend )
            return;

        m_browser_configured = true;

      #if USING_WEBVIEW_EDGE
        ICoreWebView2* webView2 = static_cast<ICoreWebView2*>(nativeBackend);
        HRESULT hr;
        wxCOMPtr<ICoreWebView2Settings> settings;

        hr = webView2->get_Settings(&settings);
        if ( FAILED(hr) )
        {
            wxLogError(_("Could not obtain WebView2Settings (error code 0x%08lx)."), (long)hr);
            return;
        }
        settings->put_IsBuiltInErrorPageEnabled(FALSE);
        settings->put_IsZoomControlEnabled(FALSE);

        wxCOMPtr<ICoreWebView2Settings3> settings3;

        hr = settings->QueryInterface(wxIID_PPV_ARGS(ICoreWebView2Settings3, &settings3));
        if ( FAILED(hr) )
        {
            wxLogError(_("Could not obtain WebView2Settings3 (error code 0x%08lx)."), (long)hr);
            return;
        }
        settings3->put_AreBrowserAcceleratorKeysEnabled(FALSE);
      #elif defined(__WXGTK__)
        WebKitWebView* wkv = static_cast<WebKitWebView*>(nativeBackend);

        if ( wkv )
        {
            /* const char* allowList[] = {"file://", "null", nullptr};
            webkit_web_view_set_cors_allowlist(wkv, allowList); */
            WebKitSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(wkv));
            if ( !settings )
                wxLogError("Could not obtain WebKitSettings to allow universal access from file URLs.");
            else
                webkit_settings_set_allow_universal_access_from_file_urls(settings, true);
        }
      #endif // #if USING_WEBVIEW_EDGE
    }


    void Frame::SimulOnShowDevTools()
    {
        void* nativeBackend = m_browser->GetNativeBackend();

        if ( !nativeBackend )
        {
            return;
        }

    #if defined(__WXGTK__)
        if ( m_browser_backend == wxWebViewBackendWebKit )
        {
            WebKitWebView* wkv = static_cast<WebKitWebView*>(nativeBackend);
            if ( wkv )
            {
                WebKitSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(wkv));
                if ( !settings )
                {
                    wxLogError(_("Could not open DevTools (failed to obtain WebKitSettings)."));
                    return;
                }
                g_object_set(settings, "enable-developer-extras", TRUE, NULL);
                WebKitWebInspector *inspector = webkit_web_view_get_inspector(WEBKIT_WEB_VIEW(wkv));
                if ( !inspector )
                {
                    wxLogError(_("Could not open DevTools (failed to obtain WebKitInspector)."));
                    return;
                }
                webkit_web_inspector_show(inspector);
                return;
             }
             m_browser->ShowDevTools();
         }
    #endif // #elif defined(__WXGTK__)

    #if defined(__WXMSW__)
    if ( m_browser_backend == wxWebViewBackendEdge )
    {
      #if USING_WEBVIEW_EDGE
        ICoreWebView2* webView2 = static_cast<ICoreWebView2*>(nativeBackend);
        const HRESULT hr = webView2->OpenDevToolsWindow();

        if ( FAILED(hr) )
            wxLogError(_("Could not open DevTools window (error code 0x%08lx)."), (long)hr);
        return;
      #endif //#if USING_WEBVIEW_EDGE
    }
    #endif
        m_browser->ShowDevTools();
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

        RunHello();

        return simulPanel;
    }

    void Frame::SendStringToJS( const wxString &payload )
    {
        wxString js = wxString::Format("Foo('%s');", payload);
        m_browser->RunScript(js);
    }

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
