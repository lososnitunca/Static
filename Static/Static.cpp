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
	
	std::string m_timeFrom;
	std::string m_timeTo;
	std::string m_daily_time;
	//std::string m_group;

	std::vector<int> m_Logins{ 0 };
	std::string m_path;

public:
	std::string m_group;

	CConfig(std::string path) : m_path(path)
	{
		m_ip = "192.168.1.2";
		m_port = "443";

		m_login = 1;
		m_password = "Admin";

		m_timeFrom = "2018-01-01";
		m_timeTo = "2018-01-01";

		m_group = "demoforex";

		m_adr = m_ip + ":" + m_port;

		m_daily_time = " 23:59:59.999";

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

			if (const boost::optional<std::string> key = iniData.get_optional<std::string>("time from"))
			{
				if (key.get() != "")
				{
					m_timeFrom = key.get();
				}
			}

			if (const boost::optional<std::string> key = iniData.get_optional<std::string>("time to"))
			{
				if (key.get() != "")
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
		}
		catch (boost::property_tree::ini_parser_error& error)
		{
			iniData.put("ip", m_ip);
			iniData.put("port", m_port);
			iniData.put("login", m_login);
			iniData.put("password", m_password);
			iniData.put("time from", m_timeFrom);
			iniData.put("time to", m_timeTo);
			iniData.put("group", m_group);
			boost::property_tree::write_ini(m_path, iniData);
		}
	}

	LPCSTR adress() // ip:port function
	{
		return m_adr.c_str();
	}

	int login() // manager login function
	{
		return m_login;
	}

	LPCSTR password() // manager password function
	{
		LPCSTR password = m_password.c_str();
		return password;
	}

	char* group() // ip:port function
	{
		char group_daily[32]; 
		strcpy(group_daily, m_group.c_str());
		return group_daily;
	}

	__time32_t timeTo()
	{
		std::string strTime = m_timeTo + m_daily_time;
		std::tm tmTime = boost::posix_time::to_tm(boost::posix_time::time_from_string(strTime));
		__time32_t timeT = (((std::mktime(&tmTime)) / 86400) * 86400) + 86400;
		return timeT;
	}

	__time32_t timeFrom()
	{
		std::string strTime = m_timeFrom + m_daily_time;
		std::tm tmTime = boost::posix_time::to_tm(boost::posix_time::time_from_string(strTime));
		__time32_t timeF = ((std::mktime(&tmTime)) / 86400) * 86400;
		return timeF;
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
	DailyGroupRequest req = { "manager", conf.timeFrom(), conf.timeTo(), 10 };
	strcpy(req.name, conf.m_group.c_str());

	int logins[10] = { 1,2,3,4,5,6,7,8,9,10 };
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

	if ((cp = strrchr(chFilename, '.')) != NULL)
	{
		*cp = 0; strcat(chFilename, ".csv");
	}
	
	std::cout << chFilename << std::endl;

	std::ofstream report_file;
	report_file.open(chFilename);

	for (int i = 0; i < total; i++)
	{
		report_file << m_daily[i].login << ";login report\n";
		report_file << m_daily[i].group << "\n";
		report_file << m_daily[i].equity << "\n";
		report_file << m_daily[i].ctm << "\n";
	}

	report_file.close();

	manager->MemFree(m_daily);

	manager->Disconnect();

	system("pause");


	return 0;
}

