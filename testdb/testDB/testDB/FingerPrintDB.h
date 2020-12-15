
#ifndef FINGERPRINTDB_H
#define FINGERPRINTDB_H

#include <string>
#include <ctime>

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>


#pragma db object
	struct FingerPrintDB {
		friend class odb::access;
		unsigned int id;
		std::string fpTemplate;
	};

#pragma db member(FingerPrintDB::id) id



#endif //FINGERPRINTDB_H
