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
	}


	DataTable^ ImportEcl::ImportSchedule(String^ FilePath)
	{
		//�������ݱ�
		DataTable^ dt_production = gcnew DataTable("ProductionTable");

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

		if (File::Exists(FilePath))
		{
			FileStream^ fs = gcnew FileStream(FilePath, FileMode::Open, FileAccess::Read);
			StreamReader^ sr = gcnew StreamReader(fs);
			//ʹ��StreamReader������ȡ�ļ� 
			sr->BaseStream->Seek(0, SeekOrigin::Begin);
			String^ tmp = sr->ReadLine();
			String^ str_date="";
			DateTime^ date;
			while (tmp != nullptr)
			{
				if (tmp->Contains("SIMULATION START DATE"))
				{
					str_date = tmp->Replace("-- SIMULATION START DATE ", "")->Replace("'", "")->TrimStart()->TrimEnd()->Replace(" ", "-");;
					break;
				}
				tmp = sr->ReadLine();
			}
			while (tmp != nullptr)
			{
				if (tmp->Contains("WCONHIST"))
				{
					tmp = sr->ReadLine();
					while (true)
					{
						#pragma region ��ǰ�а�����������
						if (tmp->Contains("/"))
						{
							#pragma region ������ֹ�� ֻ��һ����/�� û������
							if (tmp->TrimStart()->TrimEnd() == "/")
							{
								break;
							}
							#pragma endregion
							#pragma region ��������ֹ��
							else
							{
								array<String^>^ datalist = tmp->TrimStart()->TrimEnd()->Split(' ');
								DataRow^ dr = dt_production->NewRow();
								dr["WellName"] = datalist[0];
								dr["Month"] = str_date;
								dr["OilRate"] = datalist[3];
								dr["WaterRate"] = datalist[4];
								dr["GasRate"] = datalist[5];
								int remainItmCnt = datalist->Length - 6;
								int skip = 0;
								for (int i = 0; i < remainItmCnt; i++)
								{
									if (datalist[i + 6]->Contains("*"))
									{
										skip += Int32::Parse(datalist[i + 6]->Replace("*", "")) -1;
									}
									else
									{
										if (i + 6 + skip == 8)
										{
											dr["THP"] = datalist[i + 6]->Replace("/","");
										}
										else if (i + 6 + skip == 9)
										{
											dr["BHP"] = datalist[i + 6]->Replace("/", "");
										}
									}
								}
								dt_production->Rows->Add(dr);
								#pragma region ���һ������ ������ֹ�����������еĺ��
								if (tmp->Replace(" ", "")->Contains("//"))
								{
									break;
								}
								#pragma endregion
							}
						#pragma endregion
						}
						#pragma endregion
						#pragma region ��ǰ��δ������������
						else
						{
							//����һ�е�����ƴ�ӵ���ǰ�����γ�����������
							tmp += sr->ReadLine();
							continue;
						}
						#pragma endregion

						tmp = sr->ReadLine();
					}
					
				}
				else if (tmp->Contains("DATES"))
				{
					str_date = sr->ReadLine()->Replace(" /","")->Replace("'", "")->TrimStart()->TrimEnd()->Replace(" ","-");
					//array<String^>^ datalist = str_date->TrimStart()->TrimEnd()->Split(' ');
					date = DateTime::Parse(str_date);
				}
				else
				{

				}
				tmp = sr->ReadLine();
			}
		}

		return dt_production;
	}
}
