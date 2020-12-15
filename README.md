# ODB_Learn
learn ODB

## 下载
https://www.codesynthesis.com/products/odb/download.xhtml
需要下载项：
1. odb-2.4.0   
    这个是odb本身的编译工具，把他的bin目录加到环境变量里面就行
2. libodb-2.4.0
3. libodb-sqlite-2.4.0
4. sqlite3 （需要.h .lib .dll文件）  
    https://www.sqlite.org/download.html  
    只能下载到dll文件，lib文件需要通过dll自己生成一下

        PS C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29333\bin\Hostx64\x64> .\lib  /out:D:\tool\sqlite3\sqlite-dll-win64-x64-3340000\sqlite3.lib /MACHINE:IX86 /DEF:D:\tool\sqlite3\sqlite-dll-win64-x64-3340000\sqlite3.def

## 编译
用vs2019打开项目进行编译
1. 先编 libodb-2.4.0
2. 再编libodb-sqlite-2.4.0 （这里有个问题，下的那个sqlite3.dll 没有编译unlock那部分，所以这里要去掉LIBODB_SQLITE_HAVE_UNLOCK_NOTIFY 这个宏
3. 接下来可以编译demo中的项目了

自己用的时候遇见了两个问题，解决耗时都比较长
- 新写项目，odb编译生成文件后，编译不过。  
    原因在于生成的ixx文件不能作为源文件，而是要作为头文件参与编译。  
    vs中将ixx文件的项类型由C/C编译器 改为C/C标头
- ODB的项目 命令行cmake编译时，编译通过，链接报错undefined reference。  
    原因：添加源文件时，未添加上cxx后缀的源文件.解决：cmake添加源文件时，通配符改为（\*.c\*）


### 新建/读取 数据库

```
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
			    
			   //创建DB文件  
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
```
### 增加记录

```
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
```
### 查找记录

```
	typedef odb::query<FingerPrintDB> query;
	typedef odb::result<FingerPrintDB> result;
	
	// find all
	{
		transaction tr(pDB->begin());
		result r(pDB->query<FingerPrintDB>());
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
		tr.commit();
	}
```
### 删除

```
        // delete
		pDB->erase<FingerPrintDB>(nSearchID);
		
```

### 修改

```
        // update
		pInfo->fpTemplate = "333333";
		pDB->update(*pInfo);
```
