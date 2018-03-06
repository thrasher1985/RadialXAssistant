#include "FoundationClasses.h"

namespace RadialXAssistant {

	using namespace System;
	using namespace System::Collections;
	using namespace System::Data;
	using namespace System::IO;

	FoundationClasses::FoundationClasses()
	{
	}


	void FoundationClasses::ExportDatatable(String^ Path, DataTable^ dt)
	{
		if (File::Exists(Path))
		{
			return;
		}

		FileStream^ fsWrite = gcnew FileStream(Path, FileMode::OpenOrCreate, FileAccess::Write);
		StreamWriter^ sw = gcnew StreamWriter(fsWrite);
		sw->Flush();
		// 使用StreamWriter来往文件中写入内容 
		sw->BaseStream->Seek(0, SeekOrigin::Begin);
		String^ HeadLine = "";
		for each (DataColumn^ var in dt->Columns)
		{
			HeadLine += "," + var->ColumnName;
		}
		HeadLine = HeadLine->TrimStart(',');
		sw->WriteLine(HeadLine);
		int clmnCount = dt->Columns->Count;
		for each (DataRow^ dr in dt->Rows)
		{
			String^ tmp = "";
			for (int i = 0; i < clmnCount; i++)
			{
				tmp += "," + dr[i];
			}
			tmp = tmp->TrimStart(',');
			sw->WriteLine(tmp);
		}
		sw->Flush();
		sw->Close();
		fsWrite->Close();
	}
}