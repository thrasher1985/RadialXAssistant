#pragma once

namespace RadialXAssistant
{
	using namespace System;
	using namespace System::Data;

	ref class ImportEcl
	{
	public:
		ImportEcl();
		DataTable ^ dt_production;
		void ImportSchedule(String^ FilePath);
		
	private:
		void readWCONHISTLine(String^ strDataLine, DateTime LastDate, DateTime CurrentDate, String^ ORate, String^ WRate, String^ GRate, String^ THP, String^ BHP, DataTable^ dt_production, DataTable^ dtproductTemp);
		void AddNewPrductLine(String ^ strDataLine, DateTime LastDate, DateTime CurrentDate, TimeSpan tspan, String^ ORate, String^ WRate, String^ GRate, DataTable^ dtproductTemp);
		void AddTmpPrductLine(String ^ strDataLine, DateTime CurrentDate, DataTable^ dtproductTemp);
	};
}

