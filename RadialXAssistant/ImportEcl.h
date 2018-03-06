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
		String ^ ORate, ^ WRate, ^ GRate;
		void readWCONHISTLine(String^ strDataLine, DateTime LastDate, DateTime CurrentDate, DataTable^ dtproductTemp);
		void AddNewPrductLine(String ^ strDataLine, DateTime LastDate, DateTime CurrentDate, TimeSpan tspan, DataTable^ dtproductTemp);
		void AddTmpPrductLine(String ^ strDataLine, DateTime CurrentDate, DataTable^ dtproductTemp);
	};
}

