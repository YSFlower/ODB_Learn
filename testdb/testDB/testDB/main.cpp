#include <memory>
#include <iostream>
#include <string>

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/connection.hxx>
#include <odb/transaction.hxx>

#include "FingerPrintDB.h"
#include "FingerPrintDB-odb.hxx"

using namespace std;
using namespace odb::core;

int main(int argc, char* argv[])
{
	string strDBName = "FPInfo.db";
	shared_ptr<database> pDB;
	try
	{
		pDB = shared_ptr<database>(new odb::sqlite::database(strDBName, SQLITE_OPEN_READWRITE));
		odb::database& oDBInfo = *pDB;
		odb::connection_ptr pConnectionPtr(pDB->connection());
		odb::transaction tr(pConnectionPtr->begin());
		tr.commit();
	}
	catch (const std::exception& e)
	{
		try
		{
			pDB = shared_ptr<database>(new odb::sqlite::database(strDBName, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE));
			odb::database& oDBInfo = *pDB;
			odb::connection_ptr pConnectionPtr(pDB->connection());
			odb::transaction tr(pConnectionPtr->begin());
			try
			{
				odb::schema_catalog::create_schema(oDBInfo);
				tr.commit();
			}
			catch (const std::exception&)
			{
				cout << "create db fail" << endl;
				return 1;
			}
			
		}
		catch (const std::exception&)
		{
			cout << "load db fail" << endl;
			return 1;
		}
	}

	{
		FingerPrintDB FP1;
		FP1.id = 1;
		FP1.fpTemplate = "1111";

		FingerPrintDB FP2;
		FP2.id = 2;
		FP2.fpTemplate = "2222";

		transaction tr(pDB->begin());
		pDB->persist(FP1);
		pDB->persist(FP2);
		tr.commit();
	}

	typedef odb::query<FingerPrintDB> query;
	typedef odb::result<FingerPrintDB> result;

	// find all
	{
		transaction tr(pDB->begin());
		result r(pDB->query<FingerPrintDB>(query::id == 1));
		if (r.empty())
		{
			cout << "not found" << endl;
		}
		for (result::iterator i(r.begin()); i != r.end(); ++i)
		{
			cout << "Hello, " << i->id << " " << i->fpTemplate << "!" << endl;
		}

		tr.commit();
	}

	// find by id
	{
		transaction tr(pDB->begin());
		unsigned int nSearchID = 1;
		auto_ptr<FingerPrintDB> pInfo(pDB->query_one<FingerPrintDB>(query::id == nSearchID));
		if (pInfo.get() == nullptr)
		{
			cout << "not found" << endl;
		}
		else
		{
			cout << "found" << endl;

			// update
			pInfo->fpTemplate = "333333";
			pDB->update(*pInfo);
			// delete
			//pDB->erase<FingerPrintDB>(nSearchID);
		}
		//nSearchID = 11;
		//auto_ptr<FingerPrintDB> pInfo1(pDB->query_one<FingerPrintDB>(query::id == nSearchID));
		//if (pInfo1.get() == nullptr)
		//{
		//	cout << "not found" << endl;
		//	try
		//	{
		//		pDB->erase<FingerPrintDB>(nSearchID);
		//	}
		//	catch (const std::exception&)
		//	{
		//		cout << "error" << endl;
		//	}
		//	
		//}
		//else
		//{
		//	cout << "found" << endl;
		//}

		tr.commit();
	}

	return 0;
}