#include "tools.h"
#include "symbolize.h"
#include "operation.h"
#include "calculator.h"






//交换矩阵的两行元素
void exchangeRow(int r1, int r2, int col,fraction ** Augument)
{
	for (int i = 0; i < col; i++)
		swap(Augument[r1][i], Augument[r2][i]);
}


//获取方程组,返回其总数
int getEqution(vector <string> *equ)     
{
	string temp;
	int position = 0;
	getline(cin, temp);
	simplize(temp);
	while (!temp.empty())
	{
		if (temp[0] != '-')
			temp.insert(0, 1, '+');
		equ->push_back(temp);
		getline(cin, temp);
		simplize(temp);
	}
	return equ->size();
}

//获取未知数，返回其总数，解决重复bug
int getVariable(vector<string> *var)  
{
	string name;
	cin  >> name;
	while (cin.peek()!='\n')                                        //利用cin.peek()函数判断是否读取完毕
	{
		var->push_back(name);
		cin >> name;
	}
	var->push_back(name);
	sort(var->begin(), var->end());                                 //清除var中可能的重复元素，避免bug
	var->erase(unique(var->begin(), var->end()), var->end());
	return var->size();
}


//处理运算顺序，将方程两端化为整式
void simplize(string & equ)
{
	string equL, equR, result;
	if (equ.empty()) return;
	int pos = equ.find('=');
	equL = equ.substr(0, pos); equR = equ.substr(pos + 1);
	fraction fL = calculate(equL);
	fraction fR = calculate(equR);
	polynomial pL = fL.numerator*fR.denominator;
	polynomial pR = fR.numerator*fL.denominator;
	stringstream stream;
	stream << pL;
	equ = stream.str();
	equ += "=";
	stream.str("");
	stream << pR;
	equ.append(stream.str());
}

//获取系数矩阵，返回是否成功,包括移项、处理系数为1等feature
bool getMatrix(vector<string>& equ, vector<string>& var, string ** Matrix)
{
	string temp;
	bool flag1 = false, flag2 = false;
	int head, cur, pos, length, equNum = equ.size(), varNum = var.size();
	//初始化，下一步设计mystod时直接空字符串赋值为0，删去这段
	for (int i = 0; i < equNum; i++)
		for (int j = 0; j < varNum + 1; j++)
			Matrix[i][j] = "0";
	for (int i = 0; i < equNum; i++)
	{
		flag1 = false;
		head = cur = 0;
		length = equ[i].length();
		while (cur < length)    //未到达该等式末尾
		{
			head = cur++;
			flag2 = false;
			while ((equ[i][cur] != '+') && (equ[i][cur] != '-') && (equ[i][cur] != '=') && cur < length) cur++;
			temp = equ[i].substr(head, cur - head);
			pos = 0;//除去代码中的空格
			while ((pos = temp.find(' ', pos)) != string::npos)
				temp.erase(pos, 1);
			pos = 0;//除去代码中的乘号(前后均为数字除外)
			while (((pos = temp.find('*', pos)) != string::npos) && (!((isdigit(temp[pos - 1])) && (isdigit(temp[pos + 1])))))
				temp.erase(pos, 1);
			for (int j = 0; j < varNum; j++)
			{
				int pos = temp.find(var[j]);
				if (pos != string::npos)
				{
					flag2 = true;                  //该项含有变量
					temp.erase(pos, var[j].size());
					if (temp == "+" || temp == "-")
						temp += "1";
					if (flag1 == true)             //该项在等号右侧且含有变量
						changeSign(temp);
					if (Matrix[i][j] == "0")
						Matrix[i][j] = temp;
					else Matrix[i][j].append(temp);
				}
			}
			if (flag2 == false)  //该项未找到变量
			{
				if (flag1 == false)
					changeSign(temp);
				if (Matrix[i][varNum] == "0")
					Matrix[i][varNum] = temp;
				else Matrix[i][varNum].append(temp);
			}
			if (equ[i][cur] == '=')
			{
				flag1 = true; cur++;
				if ((equ[i][cur] != '-') && (equ[i][cur] != '0'))
				{
					equ[i].insert(cur, 1, '+'); length++;
				}
			}
		}
	}
	return true;
}

//高斯消元法处理增广系数矩阵
bool Elimination(fraction ** Augument, int equ, int var)
{
	int i, j, k, flag = 1;
	fraction temp;
	if (equ < var)//检查方程是否欠定
	{
		cout << "方程组欠定，请输入更多方程" << endl;
		return false;
	}
	for (k = 0; k < var; k++)
	{
		flag = 1;
		//检查是否可以消元
		if (Augument[k][k].isZero())
		{
			flag = 0;
			for (i = k; i < equ; i++)
				if (!Augument[i][k].isZero())
				{
					exchangeRow(i, k, var + 1, Augument);
					flag = 1;
				}
		}
		if (!flag) {
			cout << "方程组无解！" << endl;
			return false;
		}
		//将第k行首项划为1
		temp = Augument[k][k];
		for (i = k; i <= var; i++)
			Augument[k][i] = Augument[k][i] / temp;
		//将第k列其余系数划为0
		for (i = 0; i < equ; i++)
		{
			if (i == k) continue;
			temp = Augument[i][k];
			for (j = k; j <= var; j++)
				Augument[i][j] = Augument[i][j] +(temp * Augument[k][j]).changeSign();
		}
	}
	if (equ > var)//检查方程是否超定,及是否有解
	{
		for (i = var; i < equ; i++)
		{
			fraction temp;
			for (j = 0; j < var; j++)
				temp = Augument[i][j] * Augument[j][var] + temp;
			if (!(temp + Augument[i][var].changeSign()).isZero())
			{
				cout << "方程组超定，且无解！" << endl;
				return false;
			}
		}
		cout << "方程组超定，但仍有一组解！" << endl;
	}
	return true;
}