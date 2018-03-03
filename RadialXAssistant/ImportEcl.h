#pragma once

namespace RadialXAssistant
{
	using namespace System;
	using namespace System::Data;

	ref class ImportEcl
	{
	public:
		ImportEcl();
		static DataTable^ ImportSchedule(String^ FilePath);
	};
}

