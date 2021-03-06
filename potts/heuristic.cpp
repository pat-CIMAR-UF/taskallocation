#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip> // needed for setw(int)
#include <string>
#include "stdio.h"
#include <iostream>
#include <cstdlib>
#include <Eigen/Dense>
#include <Eigen/LU> 
#include <vector>
#include <cmath>
#include <algorithm>
#include <time.h>
#include <limits>
#include "gnuplot_iostream.h"
#include <cstring>
#include <sys/stat.h>
#include <ctime>
#include <sys/types.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>
#include <csignal>
#include <map>
#include "Hungarian.h"
#include <functional>
#include <bits/stdc++.h>
using namespace std;
using namespace Eigen;

std::string outputpath = "/home/jennifer/catkin_ws/src/taskallocation/potts/OUTPUTS/";
std::string inputpath = "/home/jennifer/catkin_ws/src/taskallocation/potts/";
int nVehicles;
int nTasks;
int nDim;
int rDim;
double checkTime;
Eigen::VectorXd cTime;
Eigen::VectorXd checkTimeVec;
Eigen::VectorXd sub;
Eigen::MatrixXd DeltaMatrix;
Eigen::MatrixXd CostMatrix;
Eigen::MatrixXd TaskMatrix;
Eigen::MatrixXd EndMatrix;
Eigen::MatrixXd StartMatrix;
Eigen::MatrixXd EndHungMatrix;
Eigen::MatrixXd StartHungMatrix;
Eigen::MatrixXd TotalEndHungMatrix;
Eigen::MatrixXd TotalStartHungMatrix;
Eigen::VectorXd TVec; 
Eigen::VectorXd Edge; 
Eigen::VectorXd Edge2; 
Eigen::VectorXd Edge3; 
std::string solStrA;
std::string solStrB;std::string veh_alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
std::string task_alpha = "abcdefghijklmnopqrstuvwxyz{|}~^_`!#$&'()*+,-./:;<=>?@abcdefghijklmnopqrstuvwxyz{|}~^_`!#$&'()*+,-./:;<=>?@abcdefghijklmnopqrstuvwxyz{|}~^_`!#$&'()*+,-./:;<=>?@abcdefghijklmnopqrstuvwxyz{|}~^_`!#$&'()*+,-./:;<=>?@abcdefghijklmnopqrstuvwxyz{|}~^_`!#$&'()*+,-./:;<=>?@abcdefghijklmnopqrstuvwxyz{|}~^_`!#$&'()*+,-./:;<=>?@";
ofstream outfile14;
std::vector<char> plotVehicle;
std::vector<std::vector<char>> plotString;
std::vector<std::pair<double,double>> vehCoord;
std::vector<std::pair<double,double>> taskCoord;
std::vector<std::pair<double,double>> totalCoord;
std::map<char, std::pair<double,double>> vehMap;
std::map<char, std::pair<double,double>> taskMap;


double euclideanDistance (std::pair<double,double> a, std::pair<double,double> b)
	{
		double dist, u, v;
		u = b.first - a.first;
		v = b.second - a.second;
                //cout << "\n x2-x1 is " << u << endl;
                //cout << "\n y2-y1 is " << v << endl;
		dist = u*u + v*v; //xb-xa and yb-ya is 
		//cout << "dist is" << dist <<endl;
		dist = sqrt(dist);
		//cout << "dist is" << dist <<endl;
		return dist;
	}

	
std::tuple<std::string, std::string, int> displaySolution(Eigen::MatrixXd &VMatrix, Eigen::MatrixXd &DeltaMatrix, Eigen::VectorXd &TVec) //parses the solution
    {
        
        int indx = 0;
        int indxB = 0;
        cTime = Eigen::VectorXd(nVehicles);
        sub = VectorXd::Ones(nVehicles);
      
        cout << "\nTHE SOLUTION is: \n" << endl;
        cout << VMatrix << endl;
           
           for (int i = 0; i < nVehicles; i++)
            {
            for (int j = 0; j < nDim; j++)
            {if (VMatrix(i,j)==1)
                {indx = j;}
            }
            
            if (i == 0)
            {
                solStrA = std::string("S") + veh_alpha[i] + std::string(" -> ") + task_alpha[indx-nVehicles];
                plotVehicle.push_back((veh_alpha[i]));
                plotVehicle.push_back((task_alpha[indx-nVehicles]));
                solStrB = "max(" + std::to_string(TVec(i)) + std::string(" + ")+ std::to_string(DeltaMatrix(i,indx)) + std::string(" + ") + std::to_string(TVec(indx));
            }
            else
            {
                solStrA = solStrA + std::string(" & S") + veh_alpha[i] + std::string(" -> ") + task_alpha[indx-nVehicles];
                if (!plotVehicle.empty())
                {plotString.push_back(plotVehicle);}
                plotVehicle.clear();
                plotVehicle.push_back((veh_alpha[i]));
                plotVehicle.push_back((task_alpha[indx-nVehicles]));
                solStrB = solStrB + std::string(", ") + std::to_string(TVec(i)) + std::string(" + ")+ std::to_string(DeltaMatrix(i,indx)) + std::string(" + ") + std::to_string(TVec(indx));
            }
            cTime[i] = DeltaMatrix(i,indx) + TVec(indx) + TVec(i);
            
            while (indx <= (nDim-nVehicles-1))
            {
                 for (int j = 0; j < nDim; j++)
                    {if (VMatrix(indx,j)==1)
                        {indxB = j;}
                    }
               
                if (indxB > (nVehicles+nTasks-1))
                {
                    solStrA = solStrA + std::string(" -> E") + veh_alpha[indxB-nVehicles-nTasks];
                    plotVehicle.push_back((veh_alpha[indxB-nVehicles-nTasks]));
                    if (!plotVehicle.empty())
                    {plotString.push_back(plotVehicle);}
                    plotVehicle.clear();
                    solStrB = solStrB + std::string(" + ") + std::to_string(DeltaMatrix(indx,indxB)) + std::string(" + ") +std::to_string(TVec(i));
                    cTime[i] = cTime[i] + DeltaMatrix(indx,indxB) + TVec(i);
                    solStrB = solStrB + std::string(" = ") + std::to_string(cTime[i]);            
                }
                else
                { 
                    solStrA = solStrA + std::string(" -> ") + task_alpha[indxB-nVehicles];
                    plotVehicle.push_back((task_alpha[indxB-nVehicles]));
                    solStrB = solStrB + std::string(" + ") + std::to_string(DeltaMatrix(indx,indxB)) + std::string(" + ") + std::to_string(TVec(indxB));
                    cTime[i] = cTime[i] + DeltaMatrix(indx,indxB) + TVec(indxB);
                }
                indx = indxB;
            }
        }
        solStrB = solStrB + std::string(")");
        checkTimeVec = VectorXd(2*nVehicles);
        checkTimeVec << cTime, cTime + (nVehicles * sub);
        VectorXf::Index maxE;
        checkTime = checkTimeVec.maxCoeff(&maxE) - nVehicles;
        cout << "The tasks are ordered as:\n";
        cout << "\n" <<solStrA << endl;
        cout << "\n" <<solStrB << endl;
        cout << "\n" <<checkTime << endl;
        
        for ( std::vector<std::vector<string>>::size_type i = 0; i < plotString.size(); i++ )
        {for ( std::vector<string>::size_type j = 0; j < plotString[i].size(); j++ )
        {std::cout << plotString[i][j] << ' ';}
        std::cout << std::endl;}  
        
    std::string createFile14 = "";    
    createFile14 = outputpath + "/" + "solution" + ".txt";          
    outfile14.open(createFile14.c_str());  
    outfile14 << "\n"<<VMatrix << std::endl;
    outfile14 << "\n"<<solStrA << std::endl;
    outfile14 << "\n"<<solStrB << std::endl;
    outfile14 << "\n"<<checkTime << std::endl;
    outfile14 << "\nThe input parameters are for : " << nVehicles << " vehicles " << nTasks << " tasks taken " << endl;

    return std::make_tuple(solStrA, solStrB, checkTime);
    }
    
int main(int argc, const char* argv[])
    
    {    
    clock_t tStart = clock();
    nVehicles = atoi(argv[1]);
    nTasks = atoi(argv[2]);    
    nDim = 2*nVehicles + nTasks;; 
    rDim = nVehicles + nTasks;
    
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[40];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d_%m_%Y_%I_%M_%S", timeinfo);
        
    std::string data = "M=" + std::to_string(nVehicles) +"_N="+std::to_string(nTasks);
    outputpath.append(std::string(data));
    std::string newDir = "_new";

    std::string at = " AT TIME ";
    outputpath.append(std::string(at));
    outputpath.append(std::string(buffer));
    mkdir(outputpath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    DeltaMatrix = MatrixXd::Ones(nDim,nDim);
    TaskMatrix = MatrixXd::Ones(nTasks,nTasks);
    TVec = VectorXd(nDim);
           
    std::string folder = inputpath + data;
    std::string folder1, folder2, folder4, folder5, folder6;

    if (!strcmp(argv[3],"-random"))
    {    
        for (int i=0; i<nDim; i++)
           {
               TVec(i) = 1;
           }
        cout << "\n TVec is: \n" << TVec <<endl;
    
        std::srand((unsigned int) time(0));
        DeltaMatrix = MatrixXd::Random(nDim,nDim);
        double HI = 100; // set HI and LO according to your problem.
        double LO = 1;
        double range= HI-LO;
        DeltaMatrix = (DeltaMatrix + MatrixXd::Constant(nDim,nDim,1.)) * range/2;
        DeltaMatrix = (DeltaMatrix + MatrixXd::Constant(nDim,nDim,LO));
        cout << "\n DeltaMatrix is: \n" << DeltaMatrix <<endl;
    }
         
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     else if (!strcmp(argv[3],"-coordsRandom")) //generate random coordinates and then calculate DeltaMatrix
    {    
        for (int i=0; i<nDim; i++)
           {
               TVec(i) = 1;
           }
        cout << "\n TVec is: \n" << TVec <<endl;
    
        std::srand((unsigned int) time(0));
        
        ofstream outfile26;
        ofstream outfile27;
       
        std::string createFile26 = "";    
        createFile26 = inputpath + "/" + "vehCoord" + ".txt";             
        outfile26.open(createFile26.c_str()); 
        std::string createFile27 = "";    
        createFile27 = inputpath + "/" + "taskCoord" + ".txt";              
        outfile27.open(createFile27.c_str()); 
        
        inputpath.append(std::string (data));
        inputpath.append(std::string (newDir));
        mkdir(inputpath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
        ofstream outfile28;
        ofstream outfile29;
        std::string createFile28 = "";    
        std::string createFile29 = "";    
        createFile28 = inputpath + "/" + "vehCoord" + ".txt";             
        createFile29 = inputpath + "/" + "taskCoord" + ".txt";              
        outfile28.open(createFile28.c_str()); 
        outfile29.open(createFile29.c_str()); 
        
        ////////////////////****************************//////////////////////
        char veh = 65;
        //create a vehicle and task coordinates file and calculate deltamatrix
        for (int i=0; i<nVehicles; i++)
        {
            double item8 = rand() % 300 + (-150.0);
            double item9 = rand() % 300 + (-150.0);
            vehCoord.push_back(std::make_pair(item8,item9));
            vehMap.insert(make_pair(veh,make_pair(item8,item9)));
            outfile26 <<item8<<"\t"<< item9 <<"\n";
            outfile28 <<item8<<"\t"<< item9 <<"\n";
            veh++;
        } 
        
        for (std::map<char,std::pair<double,double>>::iterator it=vehMap.begin(); it!=vehMap.end(); ++it)
        {std::cout <<(it->first) <<"....."<< (it->second).first<<"....."<< (it->second).second << endl;}
        
        ////////////////////****************************//////////////////////
        char tas = 97;
        for (int i=0; i<nTasks; i++)
        {
            double item10 = rand() % 100 + (-50.0);
            double item11 = rand() % 100 + (-50.0);
            totalCoord.push_back(std::make_pair(item10,item11));
            taskCoord.push_back(std::make_pair(item10,item11));
            taskMap[tas]=std::make_pair(item10,item11);
            outfile27 <<item10<<"\t"<< item11 <<"\n";
            outfile29 <<item10<<"\t"<< item11 <<"\n";
            tas++;
        } 
        for (std::map<char,std::pair<double,double>>::iterator it=taskMap.begin(); it!=taskMap.end(); ++it)
        {std::cout <<(it->first) <<"....."<< (it->second).first<<"....."<< (it->second).second << endl;}
        
        ////////////////////****************************//////////////////////
        
        totalCoord.insert(totalCoord.end(), vehCoord.begin(), vehCoord.end());
        totalCoord.insert(totalCoord.begin(), vehCoord.begin(), vehCoord.end());

        for(int i = 0; i < totalCoord.size(); i++) //print all coordinates for delta matrix
        {cout << totalCoord[i].first << ", " << totalCoord[i].second << endl;}
        
        for (int i = 0; i < nDim; i++)
            for (int j = 0; j < nDim; j++)
                {
                    DeltaMatrix(i,j) = euclideanDistance(totalCoord[i],totalCoord[j]);
                }
        cout << "\n DeltaMatrix is: \n" << DeltaMatrix <<endl;
        DeltaMatrix.diagonal().array() = 10000000000;
        DeltaMatrix.leftCols(nVehicles) *= 10000000000;
        DeltaMatrix.bottomRows(nVehicles) *= 10000000000;
        DeltaMatrix.topRightCorner(nVehicles,nVehicles) = DeltaMatrix.bottomRightCorner(nVehicles,nVehicles).eval(); 
        DeltaMatrix.bottomLeftCorner(nVehicles,nVehicles) = DeltaMatrix.bottomRightCorner(nVehicles,nVehicles).eval();       
    
        ofstream outfile19;
        ofstream outfile24;
        
        std::string createFile19 = "";  
        std::string createFile24 = "";  
        
        createFile19 = inputpath + "/" + "coordDeltaMat" + ".txt"; 
        createFile24 = inputpath + "/" + "coordTVec" + ".txt";          
        
        outfile19.open(createFile19.c_str());  
        outfile24.open(createFile24.c_str());     
        
        outfile19 << DeltaMatrix << std::endl;
        outfile24 << TVec << std::endl;
        
        outfile19.close();
        outfile24.close();
        outfile28.close();
        outfile29.close();
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if (!strcmp(argv[3],"-read")) //read DeltaMatrix from the given input file
    {
        ifstream file;
        folder1 = folder+ "/tVec.txt"; 
        cout <<"\n"<<folder1<<endl;
        file.open(folder1); 
        if (file.is_open())
        {
           for (int i=0; i<nDim; i++)
           {
               double item;
               file >> item;
               TVec(i) = item;
           }
        }
        else
        {cout <<"\n tVec file not open"<<endl;
         return(0);
        }
        cout << "\n TVec is: \n" << TVec <<endl;
        ifstream file2;
        folder2 = folder+ "/deltaMat.txt"; 
        cout <<"\n"<<folder2<<endl;
        file2.open(folder2); 
        if (file2.is_open())
        {
            for (int i = 0; i < nDim; i++)
                for (int j = 0; j < nDim; j++)
                    {
                        double item2;
                        file2 >> item2;
                        DeltaMatrix(i,j) = item2;
                    }
        } 
        else
        {cout <<"\n Deltamat file not open"<<endl;
        return(0);
        }
        DeltaMatrix.diagonal().array() = 10000000000;
        DeltaMatrix.leftCols(nVehicles) *= 10000000000;
        DeltaMatrix.bottomRows(nVehicles) *= 10000000000;
        DeltaMatrix.topRightCorner(nVehicles,nVehicles) = DeltaMatrix.bottomRightCorner(nVehicles,nVehicles).eval(); 
        DeltaMatrix.bottomLeftCorner(nVehicles,nVehicles) = DeltaMatrix.bottomRightCorner(nVehicles,nVehicles).eval(); 
    
        cout << "\n DeltaMatrix is: \n" << DeltaMatrix <<endl;
        
    }
  
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if (!strcmp(argv[3],"-coordsRead")) //read coordinates from the given input file and then calculate DeltaMatrix
    {
        ifstream file6;  //read tVec from the given file
        folder6 = folder+ "/tVec.txt"; 
        cout <<"\n"<<folder6<<endl;
        file6.open(folder6); 
        if (file6.is_open())
        {
           for (int i=0; i<nDim; i++)
           {
               double item;
               file6 >> item;
               TVec(i) = item;
           }
        }
        else
        {cout <<"\n tVec file not open"<<endl;
         return(0);
        }
        
        ofstream outfile26;
        ofstream outfile27;
       
        std::string createFile26 = "";    
        createFile26 = inputpath + "/" + "vehCoord" + ".txt";             
        outfile26.open(createFile26.c_str()); 
        std::string createFile27 = "";    
        createFile27 = inputpath + "/" + "taskCoord" + ".txt";              
        outfile27.open(createFile27.c_str()); 
        
        ifstream file4; //read vehicle coordinates
        folder4 = folder+ "/vehCoord.txt"; 
        cout <<"\n"<<folder4<<endl;
        file4.open(folder4); 
        if (file4.is_open())
        {
           char veh = 65;
           for (int i=0; i<nVehicles; i++)
           {
                double item4,item6;
                file4 >> item4;
                file4 >> item6;
                vehCoord.push_back(std::make_pair(item4,item6));
                vehMap.insert(make_pair(veh,make_pair(item4,item6)));
                outfile26 <<item4<<"\t"<< item6 <<"\n";
                veh++;
           }
        }
        else
        {cout <<"\n vehCoord file not open"<<endl;
         return(0);
        }
        
        cout << "\n*****************"<<endl;
        for(int i = 0; i < vehCoord.size(); i++)
        {cout << vehCoord[i].first << ", " << vehCoord[i].second << endl;}
        cout << "\n*****************"<<endl;
     
        for(map<char, pair<double,double> >::const_iterator it = vehMap.begin();it != vehMap.end(); ++it)
        {std::cout << it->first << " " << it->second.first << " " << it->second.second << "\n";}
        
        ifstream file5; //read task coordinates
        folder5 = folder+ "/taskCoord.txt"; 
        cout <<"\n"<<folder5<<endl;
        file5.open(folder5); 
        if (file5.is_open())
        {
            char tas=97;
            for (int i=0; i<nTasks; i++)
            {    
                double item5,item7;
                file5 >> item5;
                file5 >> item7;
                taskCoord.push_back(std::make_pair(item5,item7));
                taskMap.insert(make_pair(tas,make_pair(item5,item7)));
                outfile27 <<item5<<"\t"<< item7 <<"\n";
                tas++;
            }
            
        }
        else
        {cout <<"\n taskCoord file not open"<<endl;
         return(0);
        }
        
        cout << "\n*****************"<<endl;
        for(int i = 0; i < taskCoord.size(); i++)
        {cout << taskCoord[i].first << ", " << taskCoord[i].second << endl;}
        cout << "\n*****************"<<endl;
     
        for(map<char, pair<double,double> >::const_iterator it = taskMap.begin();it != taskMap.end(); ++it)
        {std::cout << it->first << " " << it->second.first << " " << it->second.second << "\n";}
        
        totalCoord = taskCoord;
        totalCoord.insert(totalCoord.begin(), vehCoord.begin(), vehCoord.end());
        totalCoord.insert(totalCoord.end(), vehCoord.begin(), vehCoord.end());

        cout << "\n*****************"<<endl;
        for(int i = 0; i < totalCoord.size(); i++)
        {cout << totalCoord[i].first << ", " << totalCoord[i].second << endl;}
        cout << "\n*****************"<<endl;
        
        //calculate DeltaMatrix from the coordinates
        
        for (int i = 0; i < nDim; i++)
                for (int j = 0; j < nDim; j++)
                {
                    DeltaMatrix(i,j) = euclideanDistance(totalCoord[i],totalCoord[j]);
                }
        
        cout <<"\n Delta Matrix generated using the graph coordinates and euclideanDistance \n" << DeltaMatrix << endl; 
        DeltaMatrix.diagonal().array() = 10000000000;
        DeltaMatrix.leftCols(nVehicles) *= 10000000000;
        DeltaMatrix.bottomRows(nVehicles) *= 10000000000;
        DeltaMatrix.topRightCorner(nVehicles,nVehicles) = DeltaMatrix.bottomRightCorner(nVehicles,nVehicles).eval(); 
        DeltaMatrix.bottomLeftCorner(nVehicles,nVehicles) = DeltaMatrix.bottomRightCorner(nVehicles,nVehicles).eval(); 
    
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else 
    {cout << "\n Invalid option: " << argv[3] << "      exiting....\n";
                return(0);
    }
   
    DeltaMatrix.diagonal().array() = 10000000000;
    DeltaMatrix.leftCols(nVehicles) *= 10000000000;
    DeltaMatrix.bottomRows(nVehicles) *= 10000000000;
    DeltaMatrix.topRightCorner(nVehicles,nVehicles) = DeltaMatrix.bottomLeftCorner(nVehicles,nVehicles).eval();       

    ofstream outfile1;
    std::string createFile1 = "";    
    createFile1 = outputpath + "/" + "tVec" + ".txt";          
    outfile1.open(createFile1.c_str());     
    outfile1 << TVec << std::endl;
    outfile1.close();
    
    ofstream outfile2;
    std::string createFile2 = "";    
    createFile2 = outputpath + "/" + "deltaMat" + ".txt";          
    outfile2.open(createFile2.c_str());     
    outfile2 << DeltaMatrix << std::endl;
    outfile2.close();
    
    cout << "\n Updated DeltaMatrix/Adjacency Matrix is: \n" << DeltaMatrix << endl;    
    cout << "\n ///////////////////////////////////////////////////////////////////// " << endl;
    
    Eigen::MatrixXd TaskMatrix = DeltaMatrix.block(nVehicles,nVehicles,nTasks,nTasks);
    std::cout <<"TaskMatrix is\n" << TaskMatrix <<endl;
    
    int node = 0; // every task loop
    int start_node; //start node of every task loop
    std::vector<std::vector<int>> paths_list; //paths of each of the task
    std::vector <double> values; //minimum values of all the paths
    std::vector<std::vector<double>> paths_values_list; //paths of each of the task

    LABEL0:
    start_node = node;
    std::vector<int> visited; //Global unvisited vertices vector
    std::vector <int> paths;
    paths.push_back(node);
    std::vector <double>value_paths; //stores the values of each of the path of each task
    double sum_of_elems = 0;
    int visitNo = 0;
    LABEL: 
    visitNo++;
    visited.push_back(start_node);
    Edge = VectorXd((nTasks)); 
    for(int i = 0; i < ((nTasks)); i++)
        {
            if (start_node==i)
                Edge(i) = INT_MAX;
            else if (std::find(visited.begin(),visited.end(),i) != visited.end() )
                Edge(i) = INT_MAX;
            else
            {Edge(i) = TaskMatrix(start_node,i);} //min dist from start to each of neighbour
    cout<<"\nEdge is"<<Edge(i)<<endl;
        } 
    
    MatrixXf::Index minI;
    double minValue = Edge.minCoeff(&minI);
    cout<<"\nminVal is"<<minValue<<endl;
    cout<<"\nminI is"<<minI<<endl;
    paths.push_back(minI);
    value_paths.push_back(minValue);
    cout<<"\nvisited vertices are"<<visitNo<<endl;    
    start_node = minI;
    //value of the path    
    
    if (visitNo == (nTasks-1))
        {node++;
        paths_list.push_back(paths);
        paths_values_list.push_back(value_paths);
        std::for_each(value_paths.begin(), value_paths.end(), [&] (double n) 
        {sum_of_elems += n;});
        values.push_back(sum_of_elems);
        cout << "\n////////////////////////////////////////////////////////////////" << endl;
        if (node == (nTasks))
            {goto LABEL1;} //paths for all the tasks found
        else
            goto LABEL0; //run for the next task available
        }
    else
        {goto LABEL;} //run till all the tasks are done for that task loop
    LABEL1:

    for ( std::vector<std::vector<int>>::size_type i = 0; i < paths_list.size(); i++ )
    {for ( std::vector<int>::size_type j = 0; j < paths_list[i].size(); j++ )
    {std::cout << paths_list[i][j] << ' ';}
    std::cout << std::endl;}
    cout << "\n////////////////////////////////////////////////////////////////" << endl;
       
    for ( std::vector<std::vector<int>>::size_type i = 0; i < paths_values_list.size(); i++ )
    {for ( std::vector<int>::size_type j = 0; j < paths_values_list[i].size(); j++ )
    {std::cout << paths_values_list[i][j] << ' ';}
    std::cout << std::endl;}
    cout << "\n////////////////////////////////////////////////////////////////" << endl;

    std::cout << "The smallest path is " << *std::min_element(std::begin(values),std::end(values)) << '\n';
    auto smallest = std::min_element(values.begin(), values.end() );
    double avg1 = std::accumulate(values.begin(), values.end(), 0LL) / values.size();
    std::cout << "The avg is: " << avg1 << endl;
    std::vector<double>::iterator first = values.begin();
    int  path_index = std::distance(first,smallest);
    std::cout << "The distance is: " << path_index << endl;
  
    for ( std::vector<int>::size_type j = 0; j < nTasks; j++ )
    {std::cout << paths_list[path_index][j] << ' ';}
    std::cout << std::endl; // the path array
             
    for ( std::vector<int>::size_type j = 0; j < paths_values_list[path_index].size(); j++ )
    {std::cout << paths_values_list[path_index][j] << ' ';} //the path array with values
    
    double chunk_length = *smallest/(nVehicles);
    cout << "\nchunk_length " << chunk_length<< endl;
    cout << "\n////////////////////////////////////////////////////////////////" << endl;
    
    //create a tuple for holding path values and nodes
    std::vector <tuple<double, int, int> > task_map;

    for (int i = 0; i< paths_values_list[path_index].size(); i++)
    {task_map.push_back(std::make_tuple (paths_values_list[path_index][i], paths_list[path_index][i], paths_list[path_index][i+1] ));}
   
    //sorting them according to size in descending order
    sort(task_map.begin(), task_map.end()); 
    reverse(task_map.begin(), task_map.end()); 

	//printing them
    std::cout << "taskMap contains:\n";
    for(auto const &i : task_map)
        {cout<<get<0>(i)<<" ==> "<<get<1>(i)<<"  "<<get<2>(i)<<endl;}
    cout << "\n////////////////////////////////////////////////////////////////" << endl;
    
    //we cut the (n-1) biggest chunks which does not have any repeated nodes
    vector<int> cuts; //should be equal to (n-1)
    int cutNo = nVehicles-1;
    vector <vector <double>> chunks;
	vector<int> recheck;
    vector<int> recheck2;
    int scount = 0;
    int exp_cuts = (nVehicles-1);
    vector<double> temp_path;
    
    //checking how many repeats of the nodes
    for (auto it=task_map.begin(); it!=task_map.end(); ++it)
      {
		recheck.push_back(get<2>(*it));
		std::vector<int>::iterator i1 = std::find(std::begin(recheck), std::end(recheck), get<1>(*it));
		if (i1 != std::end(recheck)) 
			{++scount;}
	  }

	cout << "\nscount is:" << scount <<endl;

   if (nVehicles == nTasks)
    {
		for (int i =0; i< nVehicles; ++i)
		{cuts.push_back(i);
		cout <<cuts[i] << " ";}
		
    cout <<"\nNo of cuts:"<<cutNo << endl;
	}
	else
	{
        for (auto itr=task_map.begin(); itr!=task_map.end(); ++itr)
		{
			if (abs(exp_cuts - scount) < (nVehicles/2))    
			{
				//check if the biggest chunk node (get<1>(*itr)) value is in the paths_list
				std::vector<int>::iterator is = std::find(std::begin(paths_list[path_index]), std::end(paths_list[path_index]), get<1>(*itr)); 
				if (is != std::end(paths_list[path_index])) //if found
					{
						int ind = std::distance(paths_list[path_index].begin(), is); //find the index to add it to cut
						cout << "..... " << get<1>(*itr) << " and found at " << ind << endl;
						cuts.push_back(ind);
					}
			}
			else
			{
				std::vector<int>::iterator i3 = std::find(std::begin(recheck2), std::end(recheck2), get<2>(*itr));
				if (i3 != std::end(recheck2)) 
				{
					//do nothing  
					cout << "here" << endl;
				}
				else
				{
					recheck2.push_back(get<2>(*itr)); //check if the second value of the biggest node is there in the present search
					std::vector<int>::iterator i2 = std::find(std::begin(recheck2), std::end(recheck2), get<1>(*itr));
						if (i2 != std::end(recheck2)) 
							{
								cout << "here2" << endl;
							}
						else
						{
							std::vector<int>::iterator is = std::find(std::begin(paths_list[path_index]), std::end(paths_list[path_index]), get<1>(*itr));
							if (is != std::end(paths_list[path_index])) 
							{   int ind = std::distance(paths_list[path_index].begin(), is);
								cout << "searching " << get<1>(*itr) << " and found at " << ind << endl;
								cuts.push_back(ind);
								recheck2.push_back(get<1>(*itr));
							}
						}
				}
			}			
			}
	
    cout << "\nCuts original size" << endl;
    for (int i=0; i<cuts.size(); i++) 
    {cout <<cuts[i] << " ";} 
     
    cout << "\nCuts after pruning" << endl;
	//cuts.erase(cuts.begin(), cuts.size() > (nVehicles-1) ?  cuts.begin() + (nVehicles-1) : cuts.end() );
    cuts.erase(cuts.begin()+(nVehicles-1), cuts.end());
    for (int i=0; i<cuts.size(); i++) 
    {cout <<cuts[i] << " ";} 
                
    cout << "\nCuts after pruning and sorting" << endl;
    sort(cuts.begin(), cuts.end());
    for (int i=0; i<cuts.size(); i++) 
    {cout <<cuts[i] << " ";} 
	
	cutNo = cuts.size();
    cout <<"\nNo of cuts:"<<cutNo << endl;
}    

    int i =0;
    for ( std::vector<int>::size_type j = 0; j < paths_list.size(); j++ )
        {  
			if (i < cuts.size())
				{temp_path.push_back(paths_list[path_index][j]);
					if (j == cuts[i])
						{ chunks.push_back(temp_path); i++;
						temp_path.clear();}
				}
			if (i >= cuts.size()) //the last chunk
				{temp_path.push_back(paths_list[path_index][j+1]);
				if (j == paths_list.size()-2)
				{chunks.push_back(temp_path);}
				}
        }
        
    cout <<"\nChunks are"<< endl;
    for ( std::vector<std::vector<int>>::size_type i = 0; i < chunks.size(); i++ )
    {for ( std::vector<int>::size_type j = 0; j < chunks[i].size(); j++ )
    {std::cout << chunks[i][j] << ' ';}
    std::cout << std::endl;}
    
    //start points
    vector<int> start_points;
    vector<int> end_points;
    vector<int> sh_points;
    vector<vector<int>> start_hung_points;

    cout <<"\nStart and End points are"<< endl;
    for ( std::vector<std::vector<int>>::size_type i = 0; i < chunks.size(); i++ )
    {start_points.push_back(chunks[i][0]);
        cout << start_points[i]<<endl;
    end_points.push_back(chunks[i][(chunks[i].size())-1]);
        cout << end_points[i]<<endl;
    }
    
    cout <<"\nStart Hung points are"<< endl;
    for ( std::vector<std::vector<int>>::size_type i = 0; i < chunks.size(); i++ )
    {   
		if ((chunks[i].size())-2 > 0)
			{for ( std::vector<std::vector<int>>::size_type j = 1; j < (chunks[i].size())-1; j++ )
            {sh_points.push_back(chunks[i][j]);}}
        else if ((chunks[i].size())-1 == 0)
            {//sh_points.push_back(chunks[i][0]);
            } 
        else
        {}
        start_hung_points.push_back(sh_points);
        sh_points.clear();
    }
     
    for ( std::vector<std::vector<int>>::size_type i = 0; i < start_hung_points.size(); i++ )
    {for ( std::vector<int>::size_type j = 0; j < start_hung_points[i].size(); j++ )
    {std::cout << start_hung_points[i][j] << ' ';}
    std::cout << std::endl;}
    
    //std::cout <<"DeltaMatrix is\n" << DeltaMatrix <<endl;
    Eigen::MatrixXd StartMatrix = DeltaMatrix.block(0,nVehicles,nVehicles,nTasks);
    std::cout <<"\nStartMatrix is\n" << StartMatrix <<endl;
    
    Eigen::MatrixXd EndMatrix = DeltaMatrix.block(nVehicles,nVehicles+nTasks,nTasks,nVehicles);
    std::cout <<"\nEndMatrix is\n" << EndMatrix <<endl;
    
    Eigen::MatrixXd StartHungMatrix = MatrixXd::Zero(nVehicles,nVehicles);
    Eigen::MatrixXd EndHungMatrix = MatrixXd::Zero(nVehicles,nVehicles);
    Eigen::MatrixXd TotalStartHungMatrix = MatrixXd::Zero(nVehicles,nVehicles);
    
    std::vector<std::vector<double>> v1; //double rows of start hung matrix
    std::vector<std::vector<double>> v2; //double rows of end hung matrix
    std::vector<double> v3; //rows of start hung matrix
    std::vector<double> v4; //rows of end hung matrix
    
    std::cout <<"\nTaskMatrix is\n" << TaskMatrix <<endl;
    
     for (int i = 0; i < cutNo+1; i++)
        {
            for (int j = 0; j < cutNo+1; j++)
            {
                StartHungMatrix(i,j) =  StartMatrix(j,start_points[i]);  
                v3.push_back(StartMatrix(j,start_points[i]));
            }
            v1.push_back(v3);
            v3.clear();
        }
    std::cout <<"\nStartHungMatrix is\n" << StartHungMatrix <<endl;
    
    for (int i = 0; i < cutNo+1; i++)
        {
            for (int j = 0; j < cutNo+1; j++)
            {
                 EndHungMatrix(i,j) = EndMatrix(end_points[i],j); 
                 v4.push_back(EndMatrix(end_points[i],j));
            }
            v2.push_back(v4);
            v4.clear();
        }
    std::cout <<"\nEndHungMatrix is\n" << EndHungMatrix <<endl;
    
    
    std::cout <<"\nStartHungMatrix in vector form is" <<endl;
    //start hung matrix in vector form
    for ( std::vector<std::vector<int>>::size_type i = 0; i < v1.size(); i++ )
    {for ( std::vector<int>::size_type j = 0; j < v1[i].size(); j++ )
    {std::cout << v1[i][j] << ' ';}
    std::cout << std::endl;}    
    
    
    std::cout <<"\nEndHungMatrix in vector form is" <<endl;
    //end hung matrix in vector form
    for ( std::vector<std::vector<int>>::size_type i = 0; i < v2.size(); i++ )
    {for ( std::vector<int>::size_type j = 0; j < v2[i].size(); j++ )
    {std::cout << v2[i][j] << ' ';}
    std::cout << std::endl;}    
     
    HungarianAlgorithm HungAlgo;
    vector<int> assignment;
    vector<int> assignment2;
    double start_cost = HungAlgo.Solve(v1, assignment);
    double end_cost = HungAlgo.Solve(v2, assignment2);
    
    cout << "\n////////////ASSIGNMENT OF START POINTS////////////////" << endl;
    for (unsigned int x = 0; x < v1.size(); x++)
    {
     std::cout << x << " assigned to " << assignment[x] << "\n";
    }
    cout << "\n///////////ASSIGNMENT OF END POINTS/////////////////////" << endl;
    for (unsigned int x = 0; x < v2.size(); x++)
    {
     std::cout << x << " assigned to " << assignment2[x] << "\n";
    }
    cout << "\n////////////////////////////////////////////////////" << endl;
    
    int z =0;
    for ( std::vector<int>::size_type i = 0; i < chunks.size(); i++ )
	{
        chunks[i].insert(chunks[i].begin(),(assignment[z]-nVehicles));
		for ( std::vector<int>::size_type j = 0; j < chunks[i].size(); j++ )
			{  chunks[i][j] = chunks[i][j] + nVehicles;}
        chunks[i].push_back(assignment2[z]+nVehicles+nTasks);
        z++;
    }
           
    for ( std::vector<std::vector<int>>::size_type i = 0; i < chunks.size(); i++ )
    {for ( std::vector<int>::size_type j = 0; j < chunks[i].size(); j++ )
    {std::cout << chunks[i][j] << ' ';}
    std::cout << std::endl;}
    
    cout << "\n////////////////////////////////////////////////////////////////" << endl;
    cout << "\nTotal number of vehicles are: " << chunks.size() <<endl;
    int count2 = 0;
    for ( std::vector<std::vector<int>>::size_type i = 0; i < chunks.size(); i++ )
    {std::cout << "\nVehicle " << (chunks[i][0] + 1)<<" does "<<chunks[i].size()-2 <<" tasks"<<endl;
    count2 = count2 + chunks[i].size()-2;}
    cout << "\nTotal number of tasks are: " << count2 <<endl;
    cout << "\n////////////////////////////////////////////////////////////////" << endl;

    Eigen::MatrixXd CostMatrix = MatrixXd::Zero(nDim,nDim);  
    vector<double>total_cost;
    double cost;
    
    for ( std::vector<int>::size_type i = 0; i < nVehicles; i++ )
    {
        cost=0;
        for ( std::vector<int>::size_type j = 0; j < chunks[i].size()-1; j++ )
        {cost = cost+DeltaMatrix(chunks[i][j],chunks[i][j+1]);
         CostMatrix(chunks[i][j],chunks[i][j+1]) = 1;
        }
        cost = cost + chunks[i].size()-2;
        total_cost.push_back(cost);
        cout << "\nTotal cost is " <<total_cost[i] << endl;
    }   
    
    cout << "\nCostMatrix is:\n" << CostMatrix << endl;
    cout << "\n////////////////////////////////////////////////////////////////" << endl;

    for (unsigned int x = 0; x < total_cost.size(); x++)
    {
     std::cout << total_cost[x] << "\n";
    }
    cout << "\n////////////////////////////////////////////////////////////////" << endl;
           
    displaySolution(CostMatrix, DeltaMatrix, TVec);//Printing out the solution

    printf("\nTotal computational time taken: %.2f\n", (((double)(clock() - tStart)/CLOCKS_PER_SEC)));
    outfile14 << "\nTotal computational time taken:" << ((double)(clock() - tStart)/CLOCKS_PER_SEC) <<endl;
    outfile14.close();
    
    ofstream outfile15;
    std::string createFile15 = "";    
    createFile15 = outputpath + "/" + "Vmat" + ".txt";          
    outfile15.open(createFile15.c_str()); 
    outfile15 << CostMatrix <<endl;
    
    Gnuplot gp;
    gp << "plot 'taskCoord.txt' using 1:2:(sprintf('(%d, %d)', $1, $2)) with points notitle \n";
            //create files separate for each vehicle and plot them on the existing plot
            for (int i = 0; i < nVehicles; i++)
            {   
                ofstream outfile25;
                outfile25.open (std::to_string(i) + ".txt");
                for ( std::vector<char>::size_type j = 0; j < plotString[i].size(); j++ )
                {   
                    std::map<char,std::pair<double,double>>::iterator it;
                    it = vehMap.find(plotString[i].at(j));
                    if (it != vehMap.end() )
                        { 
                            outfile25 << (it->second).first << " ";
                            outfile25 << (it->second).second << endl;
                        }
                    it = taskMap.find(plotString[i].at(j));
                    if (it != taskMap.end() )
                        { 
                            outfile25 << (it->second).first << " ";
                            outfile25 << (it->second).second << endl;
                        }
                }
            outfile25.close();
            gp << "replot '" << i <<".txt'" <<" using 1:2:(sprintf('(%d, %d)', $1, $2)) with lines notitle \n";               
            }
            gp << "replot 'vehCoord.txt' using 1:2:(sprintf('(%d, %d)', $1, $2)) with points notitle \n"; 
    }
    
