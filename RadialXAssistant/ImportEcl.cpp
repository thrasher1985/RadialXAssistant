#include "ImportEcl.h"

namespace RadialXAssistant
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::IO;
	using namespace System::Text;

	ImportEcl::ImportEcl()
	{
		//创建油井月报数据表
		dt_production = gcnew DataTable("ProductionTable");

		DataColumn^ ClmnWellID = gcnew DataColumn();
		ClmnWellID->DataType = System::Type::GetType("System.String");
		ClmnWellID->ColumnName = "WellName";
		dt_production->Columns->Add(ClmnWellID);

		DataColumn^ ClmnDate = gcnew DataColumn();
		ClmnDate->DataType = System::Type::GetType("System.String");
		ClmnDate->ColumnName = "Month";
		dt_production->Columns->Add(ClmnDate);

		DataColumn^ ClmnOilRate = gcnew DataColumn();
		ClmnOilRate->DataType = System::Type::GetType("System.Double");
		ClmnOilRate->ColumnName = "OilRate";
		dt_production->Columns->Add(ClmnOilRate);

		DataColumn^ ClmnWaterRate = gcnew DataColumn();
		ClmnWaterRate->DataType = System::Type::GetType("System.Double");
		ClmnWaterRate->ColumnName = "WaterRate";
		dt_production->Columns->Add(ClmnWaterRate);

		DataColumn^ ClmnGasRate = gcnew DataColumn();
		ClmnGasRate->DataType = System::Type::GetType("System.Double");
		ClmnGasRate->ColumnName = "GasRate";
		dt_production->Columns->Add(ClmnGasRate);

		DataColumn^ ClmnBHP = gcnew DataColumn();
		ClmnBHP->DataType = System::Type::GetType("System.Double");
		ClmnBHP->ColumnName = "BHP";
		dt_production->Columns->Add(ClmnBHP);

		DataColumn^ ClmnTHP = gcnew DataColumn();
		ClmnTHP->DataType = System::Type::GetType("System.Double");
		ClmnTHP->ColumnName = "THP";
		dt_production->Columns->Add(ClmnTHP);
	}

	void ImportEcl::ImportSchedule(String^ FilePath)
	{
		if (File::Exists(FilePath))
		{
			FileStream^ fs = gcnew FileStream(FilePath, FileMode::Open, FileAccess::Read);
			StreamReader^ sr = gcnew StreamReader(fs);
			//使用StreamReader类来读取文件 
			sr->BaseStream->Seek(0, SeekOrigin::Begin);
			String^ tmp = sr->ReadLine();
			String^ str_date;
			DateTime LastDate;
			DateTime CurrentDate;
			String^ ORate;
			String^ WRate;
			String^ GRate;
			String^ THP;
			String^ BHP;
			DataTable^ dtproductTemp;
			dtproductTemp = dt_production->Clone();
			#pragma region 获取模拟起始时间
			while (tmp != nullptr)
			{
				if (tmp->Contains("SIMULATION START DATE"))
				{
					str_date = tmp->Replace("-- SIMULATION START DATE ", "")->Replace("'", "")->TrimStart()->TrimEnd()->Replace(" ", "-");;
					LastDate = DateTime::Parse(str_date);
				}
				if (tmp->Contains("DATES"))
				{
					str_date = tmp->Replace(" /", "")->Replace("'", "")->TrimStart()->TrimEnd()->Replace(" ", "-");
					CurrentDate = DateTime::Parse(str_date);
					if (&LastDate == nullptr)
					{
						LastDate = CurrentDate.AddMonths(-1);
					}
					if (LastDate.Day != 1)
					{
						LastDate = DateTime::Parse(LastDate.Year.ToString() + "-" + LastDate.Month.ToString() + "-1");
					}
					break;
				}
				tmp = sr->ReadLine();
			}
			#pragma endregion

			while (tmp != nullptr)
			{
				if (tmp->Contains("WCONHIST"))
				{
					tmp = sr->ReadLine();
					while (true)
					{
						#pragma region 当前行包含完整数据
						if (tmp->Contains("/"))
						{
							#pragma region 数据终止行 只有一个“/” 没有数据
							if (tmp->TrimStart()->TrimEnd() == "/")
							{
								break;
							}
							#pragma endregion
							#pragma region 非数据终止行
							else
							{
								readWCONHISTLine(tmp, LastDate, CurrentDate, ORate, WRate, GRate, THP, BHP, dt_production, dtproductTemp);
								//readWCONHISTLine(String ^ strDataLine, DateTime LastDate, DateTime CurrentDate, String^ ORate, String^ WRate, String^ GRate, String^ THP, String^ BHP, DataTable ^ dt_production, DataTable^ dtproductTemp)
								#pragma region 最后一行数据 数据终止符放在数据行的后边
								if (tmp->Replace(" ", "")->Contains("//"))
								{
									break;
								}
								#pragma endregion
							}
							#pragma endregion
						}
						#pragma endregion
						#pragma region 当前行未包含完整数据
						else
						{
							//将下一行的数据拼接到当前行以形成完整的数据
							tmp += sr->ReadLine();
							continue;
						}
						#pragma endregion

						tmp = sr->ReadLine();
					}
				}
				else if (tmp->Contains("DATES"))
				{
					str_date = sr->ReadLine()->Replace(" /", "")->Replace("'", "")->TrimStart()->TrimEnd()->Replace(" ", "-");
					if (&LastDate == nullptr)
					{
						LastDate = DateTime::Parse(str_date);
					}
				}
				else
				{

				}
				tmp = sr->ReadLine();
			}
		}
	}

	void ImportEcl::readWCONHISTLine(String ^ strDataLine, DateTime LastDate, DateTime CurrentDate, String^ ORate, String^ WRate, String^ GRate, String^ THP, String^ BHP, DataTable ^ dt_production, DataTable^ dtproductTemp)
	{
		array<String^>^ datalist = strDataLine->TrimStart()->TrimEnd()->Split(' ');

		if (dtproductTemp->Select("WellName= '" + datalist[0] + "'")->Length>0)
		{
			
			System::TimeSpan tspan = CurrentDate.Subtract(LastDate);
			//按月模拟，同一口井的两个连续生产数据的时间间隔不会大于31天
			//大于31天时属于关井情况
			if (CurrentDate.Day != 1 && tspan.Days <= 31)
			{
				if (CurrentDate.Month>LastDate.Month)
				{
					DateTime datetimetmp = DateTime::Parse(LastDate.AddMonths(1).Year.ToString()
						+ "-" + LastDate.AddMonths(1).Month + "-1");
					tspan = datetimetmp.Subtract(LastDate);
					AddNewPrductLine(strDataLine, LastDate, datetimetmp, tspan, ORate, WRate, GRate, dtproductTemp);
					LastDate = datetimetmp;
					AddTmpPrductLine(strDataLine, CurrentDate, dtproductTemp);
					tspan = CurrentDate.Subtract(LastDate);
				}
				DataRow^ dr0 = dtproductTemp->Select("WellName= '" + datalist[0] + "'")[0];
				dr0["OilRate"] = double::Parse(dr0["OilRate"]->ToString()) + double::Parse(ORate)*tspan.Days;
				dr0["WaterRate"] = double::Parse(dr0["WaterRate"]->ToString()) + double::Parse(WRate)*tspan.Days;
				dr0["GasRate"] = double::Parse(dr0["GasRate"]->ToString()) + double::Parse(GRate)*tspan.Days;
			}
			else if (CurrentDate.Day == 1 && tspan.Days <= 31)
			{
				AddNewPrductLine(strDataLine, LastDate, CurrentDate, tspan, ORate, WRate, GRate, dtproductTemp);
			}
			#pragma region 关井的情况
			else
			{
				DateTime datetimetmp = DateTime::Parse(LastDate.AddMonths(1).Year.ToString()
					+ "-" + LastDate.AddMonths(1).Month + "-1");
				tspan = datetimetmp.Subtract(LastDate);
				AddNewPrductLine(strDataLine, LastDate, datetimetmp, tspan, ORate, WRate, GRate, dtproductTemp);
				DataRow^ dr0 = dtproductTemp->Select("WellName= '" + datalist[0] + "'")[0];
				dtproductTemp->Rows->Remove(dr0);
			}
			#pragma endregion

			ORate = datalist[3];
			WRate = datalist[4];
			GRate = datalist[5];
		}
		else
		{
			AddTmpPrductLine(strDataLine, CurrentDate, dtproductTemp);
		}
	}
}


void RadialXAssistant::ImportEcl::AddNewPrductLine(String ^ strDataLine, DateTime LastDate, DateTime CurrentDate, TimeSpan tspan, String^ ORate, String^ WRate, String^ GRate, DataTable^ dtproductTemp)
{
	array<String^>^ datalist = strDataLine->TrimStart()->TrimEnd()->Split(' ');
	DataRow^ dr = dt_production->NewRow();
	DataRow^ dr0 = dtproductTemp->Select("WellName= '" + datalist[0] + "'")[0];

	dr["WellName"] = datalist[0];
	dr["Month"] = LastDate.Year.ToString() + CurrentDate.Month.ToString();
	dr["OilRate"] = double::Parse(dr0["OilRate"]->ToString()) + double::Parse(ORate)*tspan.Days;
	dr["WaterRate"] = double::Parse(dr0["WaterRate"]->ToString()) + double::Parse(WRate)*tspan.Days;
	dr["GasRate"] = double::Parse(dr0["GasRate"]->ToString()) + double::Parse(GRate)*tspan.Days;
	dr["THP"] = dr0["THP"];
	dr["BHP"] = dr0["BHP"];
	dt_production->Rows->Add(dr);

	dr0["OilRate"] = 0;
	dr0["WaterRate"] = 0;
	dr0["GasRate"] = 0;
	dr0["Month"] = CurrentDate.Year.ToString() + CurrentDate.Month.ToString();
	int remainItmCnt = datalist->Length - 6;
	int skip = 0;
	for (int i = 0; i < remainItmCnt; i++)
	{
		if (datalist[i + 6]->Contains("*"))
		{
			skip += Int32::Parse(datalist[i + 6]->Replace("*", "")) - 1;
		}
		else
		{
			if (i + 6 + skip == 8)
			{
				dr0["THP"] = datalist[i + 6]->Replace("/", "");
			}
			else if (i + 6 + skip == 9)
			{
				dr0["BHP"] = datalist[i + 6]->Replace("/", "");
			}
		}
	}
}


void RadialXAssistant::ImportEcl::AddTmpPrductLine(String ^ strDataLine, DateTime CurrentDate, DataTable^ dtproductTemp)
{
	array<String^>^ datalist = strDataLine->TrimStart()->TrimEnd()->Split(' ');
	DataRow^ dr = dtproductTemp->NewRow();

	dr["WellName"] = datalist[0];
	dr["Month"] = CurrentDate.Year.ToString() + CurrentDate.Month.ToString();
	dr["OilRate"] = 0;
	dr["WaterRate"] = 0;
	dr["GasRate"] = 0;

	int remainItmCnt = datalist->Length - 6;
	int skip = 0;
	for (int i = 0; i < remainItmCnt; i++)
	{
		if (datalist[i + 6]->Contains("*"))
		{
			skip += Int32::Parse(datalist[i + 6]->Replace("*", "")) - 1;
		}
		else
		{
			if (i + 6 + skip == 8)
			{
				dr["THP"] = datalist[i + 6]->Replace("/", "");
			}
			else if (i + 6 + skip == 9)
			{
				dr["BHP"] = datalist[i + 6]->Replace("/", "");
			}
		}
	}
	dtproductTemp->Rows->Add(dr);
}
