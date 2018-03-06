#pragma once
namespace RadialXAssistant {

	using namespace System;
	using namespace System::Collections;
	using namespace System::Data;
	ref class FoundationClasses
	{
	public:
		FoundationClasses();
		static void ExportDatatable(String^ Path, DataTable^ dt);
	};
}

