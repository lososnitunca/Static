// Static.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

class CManager
{
private:
	CManagerFactory   m_factory;
	CManagerInterface *m_manager;
public:
	CManager() : m_factory("mtmanapi.dll"), m_manager(NULL)
	{
		m_factory.WinsockStartup();
		if (m_factory.IsValid() == FALSE || (m_manager = m_factory.Create(ManAPIVersion)) == NULL)
		{
			std::cout << "Failed to create MetaTrader 4 Manager API interface" << std::endl;
			return;
		}
	}
	~CManager()
	{
		if (m_manager != NULL)
		{
			if (m_manager->IsConnected())
				m_manager->Disconnect();
			m_manager->Release();
			m_manager = NULL;
		}
		m_factory.WinsockCleanup();
	}
	bool IsValid()
	{
		return(m_manager != NULL);
	}
	CManagerInterface* operator->()
	{
		return(m_manager);
	}
};


class CConfig // init class for parsing ini file
{
private:
	boost::property_tree::ptree iniData;
	
	std::string m_server;
	std::string m_adr;

	std::string m_ip;
	std::string m_port;
	
	int m_login;
	std::string m_password;
	
	time_t m_timeFrom;
	time_t m_timeTo;

	std::string m_group;

	std::vector<int> m_Logins{ 0 };
	std::string m_path;

public:
	CConfig(std::string path) : m_path(path) // we believe that we have Server, Manager, Timer, Logins sections in file
	{
		try
		{
			boost::property_tree::read_ini(m_path, iniData);

			if (const boost::optional<std::string> key = iniData.get_optional<std::string>("ip"))
			{
				if (key.get() != "")
				{
					m_ip = key.get();
				}
			}

			if (const boost::optional<std::string> key = iniData.get_optional<std::string>("port"))
			{
				if (key.get() != "")
				{
					m_port = key.get();
				}
			}

			if (const boost::optional<int> key = iniData.get_optional<int>("login"))
			{
				if (key.get() != NULL)
				{
					m_login = key.get();
				}
			}

			if (const boost::optional<std::string> key = iniData.get_optional<std::string>("password"))
			{
				if (key.get() != "")
				{
					m_password = key.get();
				}
			}

			if (const boost::optional<time_t> key = iniData.get_optional<time_t>("time from"))
			{
				if (key.get() != NULL)
				{
					m_timeFrom = key.get();
				}
			}

			if (const boost::optional<time_t> key = iniData.get_optional<time_t>("time to"))
			{
				if (key.get() != NULL)
				{
					m_timeTo = key.get();
				}
			}

			if (const boost::optional<std::string> key = iniData.get_optional<std::string> ("group"))
			{
				if (key.get() != "")
				{
					m_group = key.get();
				}
			}

			m_adr = m_ip + ":" + m_port;
			//int counter = iniLogins.size(); // init list of logins opening orders
			//m_Logins.resize(counter);
			//counter = 0;
			//for (auto&i : iniLogins)
			//{
			//	m_Logins[counter] = i.second.get<int>("");
			//	++counter;
			//}
			//std::sort(m_Logins.begin(), m_Logins.end());
		}
		catch (boost::property_tree::ini_parser_error& error)
		{
			std::cout << "No ini file or bad file structure" << std::endl << std::endl;
		}
	}

	LPCSTR ip() // ip:port function
	{
		return m_ip.c_str();
	}

	LPCSTR port() // ip:port function
	{
		return m_port.c_str();
	}

	LPCSTR adress() // ip:port function
	{
		return m_adr.c_str();
	}

	int login() // manager login function
	{
		return (m_login);
	}

	LPCSTR password() // manager password function
	{
		LPCSTR password = m_password.c_str();
		return (password);
	}

	char* group() // ip:port function
	{
		char group[32]; 
		strcpy(group, m_group.c_str());
		return (group);
	}

	/*
	int timer() // closing timer function
	{
		return m_time;
	}
	std::vector<int> logins() // list of logins opening orders function
	{
		return m_Logins;
	}*/
};


int main()
{
	//------------------------------------------------ here we make .ini file adress + name

	WCHAR  lpFilename[256];
	char chFilename[256];
	char *cp;
	GetModuleFileName(NULL, lpFilename, sizeof(lpFilename) - 5);
	WideCharToMultiByte(CP_ACP, 0, lpFilename, -1, chFilename, 256, NULL, NULL);
	if ((cp = strrchr(chFilename, '.')) != NULL) 
	{
		*cp = 0; strcat(chFilename, ".ini"); 
	}

	std::cout << chFilename << std::endl;
	//------------------------------------------------

	CConfig conf(chFilename);
	LPCSTR adr = conf.adress();
	std::cout << adr << " adress" << std::endl;

	int res = RET_ERROR;
	CManager manager;

	if ((res = manager->Connect(adr)) != RET_OK || (res = manager->Login(conf.login(), conf.password())) != RET_OK)
	{
		std::cout << "Connect to server as " << conf.login() << " failed" << std::endl;
		system("pause");
		return RET_ERROR;
	}
	

	DailyReport* m_daily = NULL;
	DailyGroupRequest req = { "manager", (time(NULL) / 86400) * 86400, ((time(NULL) / 86400) * 86400) + 86400, 10 };
	strcpy(req.name, conf.group());

	int logins[10] = {1,2,3,4,5,6,7,8,9,10};
	int total = 0;

	m_daily = manager->DailyReportsRequest(&req, logins, &total);
	if (total == NULL)
	{
		std::cout << "REQUEST'S FAILED" << std::endl;
	}
	else
	{
		std::cout << "SUCCESS: " << total << std::endl;
	}
	

	/*TradeRecord* m_stat = NULL;
	ReportGroupRequest req_2 = { "demoforex", (time(NULL) / 86400) * 86400, ((time(NULL) / 86400) * 86400) + 86400, 10 };
	int logins_2[10] = { 1,2,3,4,5,6,7,8,9,10 };
	int total_2 = 0;

	m_stat = manager->ReportsRequest(&req_2, logins_2, &total_2);
	if (total_2 == NULL)
	{
		std::cout << "REQUEST'S FAILED" << std::endl;
	}
	else
	{
		std::cout << "SUCCESS" << std::endl;
	}


	std::cout << m_stat->login << std::endl;
	std::cout << m_stat->volume << std::endl;
	std::cout << m_stat->comment << std::endl;*/

	std::cout << m_daily[0].login << std::endl;
	std::cout << m_daily[0].group << std::endl;
	std::cout << m_daily[0].balance << std::endl;

	std::cout << m_daily[1].login << std::endl;
	std::cout << m_daily[1].group << std::endl;
	std::cout << m_daily[1].deposit << std::endl;

	std::cout << m_daily[2].login << std::endl;
	std::cout << m_daily[2].group << std::endl;
	std::cout << m_daily[2].deposit << std::endl;

	std::cout << m_daily[3].login << std::endl;
	std::cout << m_daily[3].group << std::endl;
	std::cout << m_daily[3].deposit << std::endl;

	std::cout << m_daily[4].login << std::endl;
	std::cout << m_daily[4].group << std::endl;
	std::cout << m_daily[4].deposit << std::endl;

	manager->MemFree(m_daily);
	//manager->MemFree(m_stat);
	
	GetModuleFileName(NULL, lpFilename, sizeof(lpFilename) - 5);
	WideCharToMultiByte(CP_ACP, 0, lpFilename, -1, chFilename, 256, NULL, NULL);
	if ((cp = strrchr(chFilename, '.')) != NULL)
	{
		*cp = 0; strcat(chFilename, ".csv");
	}
	
	std::ofstream myfile;
	myfile.open(chFilename);

	myfile << "a,d,c,n";

	myfile.close();

	system("pause");

	manager->Disconnect();

	return 0;
}

