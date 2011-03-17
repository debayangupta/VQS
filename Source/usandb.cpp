//
//  usandb.cpp
//  VQS
//
//  Created by Azza Abouzied on 12/7/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#include <vector>
#include <typeinfo>
#include <map>
#include <string>
#include <iostream> 
#include <Schema.hpp>

#include <Scatterplot.hpp>
#include <BarChart.hpp>

#include <boost/dynamic_bitset.hpp>

int read(){
    Schema s;
    Schema::getSchema(s, "fooddb");
    
    boost::shared_ptr<Column<int> > ptr1 = s.getColumn<int>("NDB_No");
    boost::shared_ptr<Column<int> > ptr2 = s.getColumn<int>("NDB_No");
    
    boost::shared_ptr<Column<std::string> > ptr4 = s.getColumn<std::string>("Shrt_Desc");
    
    const int* data1 = ptr1->getData();
    const std::string* data2 = ptr4->getData();
    const int max = ptr2->getMax();
    
    for(int i = 0; i < 10; i++){
        std::cout << data1[i] << " - " << data2[i] << std::endl;
    }
        
    std::cout << "MAX NO: " << max << std::endl;
    
    boost::shared_ptr<Column<int> > group = s.getColumn<int>("Group_Code");
    boost::shared_ptr<Column<double> > sugar = s.getColumn<double>("Sugar_Tot");
    boost::shared_ptr<Column<double> > fat = s.getColumn<double>("Zinc");
    Scatterplot<double, double>* sc = new Scatterplot<double, double>();
    sc->set(sugar, fat);
    sc->render(std::cout, 400, 400, sugar->getMin(), sugar->getMax(),fat->getMin(),fat->getMax());
    sc->render(std::cout);
    
    BarChart<int> *bc = new BarChart<int>();
    bc->set(group);
    bc->render(std::cout);
    
    return 0;
}

int mappable(){
    std::string column_names[] = {"NDB_No",  "Shrt_Desc", "Water", "Energ_Kcal", "Protein", "Lipid_Tot", "Ash", "Carbohydrate", "Fiber_TD", "Sugar_Tot", "Calcium", "Iron", "Magnesium", "Phosphorus", "Potassium", "Sodium", "Zinc", "Copper", "Manganese", "Selenium", "Vit_C", "Thiamin", "Riboflavin", "Niacin", "Panto_acid", "Vit_B", "Folate_Tot", "Folic_acid", "Food_Folate",  "Folate_DFE", "Choline_Tot", "Vit_B12",  "Vit_A_IU", "Vit_A_RAE", "Retinol", "Alpha_Carot", "Beta_Carot", "Beta_Crypt", "Lycopene", "Lut+Zea", "Vit_E", "Vit_D_mcg", "Vit_D_IU", "Vit_K", "FA_Sat", "FA_Mono", "FA_Poly", "Cholestrl", "GmWt_1", "GmWt_Desc1", "GmWt_2", "GmWt_Desc2","Refuse_Pct", "Group_Code"};
    
    std::string col_types[] = {typeid(int).name(), typeid(mstr).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(mstr).name(), typeid(double).name(), typeid(mstr).name(),
        typeid(short).name(), typeid(int).name()};
    
    //Build map of food description codes:
    std::map<int, int> codes;
    std::ifstream fooddes;
    fooddes.open("/Users/azza/Downloads/sr23abbr/FOOD_DES.txt");
    if(fooddes.is_open()){
        std::string l;
        while(std::getline(fooddes,l)){
            int key;
            int value;
            char *tokline = (char *)malloc((strlen(l.c_str()) + 1)*sizeof(char));
            tokline = strcpy(tokline, l.c_str());
            
            char *token = strtok(tokline, "^~");
            key = atoi(token);
            
            token = strtok(NULL, "^~");
            value = atoi(token);
            
            codes[key] = value;
            
            token = NULL;
            
            free(tokline);
        }
        fooddes.close();
    }
    
    size_t cols = 54;
    size_t size = 7636;
    
    std::vector<std::string> names(54);
    std::vector<std::string> types(54);
    for(int i = 0; i < cols; i++){
        names[i] = column_names[i];
        types[i] = col_types[i];
    }
    
    
    Schema* s = new Schema("fooddbm", size, cols, names, types, true);
    
    
    void* colptrs[cols];
    for(int i = 0; i < cols; i++){
        //switch (types[i]){
        if(types[i] == typeid(mstr).name()) colptrs[i] = new mstr[size];
        else if(types[i] == typeid(int).name()) colptrs[i] = new int[size];
        else if(types[i] == typeid(short).name()) colptrs[i] = new short[size];
        else if(types[i] == typeid(double).name()) colptrs[i] = new double[size];
    }
    
    boost::dynamic_bitset<> mmaps[cols];
    for(int i = 0; i < cols; i++){
        mmaps[i].resize(size);
    }
    
    std::string line;
    std::ifstream abbrv;
    abbrv.open("/Users/azza/Downloads/sr23abbr/ABBREV.txt", std::ios::in);
    if (abbrv.is_open()){
        int i = 0;
        while (abbrv.good() && i < size){
            std::getline(abbrv,line);
            
            char *tokline = (char *)malloc(strlen(line.c_str())*sizeof(char));
            strcpy(tokline, line.c_str());
            
            char *token;
            token = strsep(&tokline, "^");
            int c = 0;
            while(token && c < (cols - 1)){
                char* tk = strtok(token, "~");
                if(tk == NULL){
                    mmaps[c][i] = 1;
                }
                else{
                    if(types[c] == typeid(mstr).name()) strcpy(((mstr*)colptrs[c])[i] , tk);
                    else if(types[c] == typeid(int).name()) ((int *) colptrs[c])[i] = atoi(tk);
                    else if(types[c] == typeid(short).name()) ((short *) colptrs[c])[i] = (short)atoi(tk);
                    else if(types[c] == typeid(double).name()) ((double *) colptrs[c])[i] = atof(tk);
                }
                c++;
                token = strsep(&tokline, "^");
            }
            ((int *)colptrs[c])[i] = codes[((int *)colptrs[0])[i]];
            i++;
        }
        abbrv.close();
    }
    
    for(int i = 0; i < cols; i++){
        if(types[i] == typeid(mstr).name()) s->writeToColumn<mstr>(names[i], (mstr *) colptrs[i], mmaps[i]);
        else if(types[i] == typeid(int).name()) s->writeToColumn<int>(names[i], (int *) colptrs[i], mmaps[i]);
        else if(types[i] == typeid(short).name()) s->writeToColumn<short>(names[i], (short *) colptrs[i], mmaps[i]);
        else if(types[i] == typeid(double).name())s->writeToColumn<double>(names[i], (double *) colptrs[i], mmaps[i]);
    }
    
    boost::shared_ptr<Column<int> > cptr1  = s->getColumn<int>(names[0]);
    boost::shared_ptr<Column<mstr> > cptr2  = s->getColumn<mstr>(names[1]);
    
    const int* data1 = cptr1->getData();
    const mstr* data2 = cptr2->getData();
    for(int i = 0; i < 10; i++){
        std::cout << data1[i] << " - " << data2[i] << std::endl;
    }
    return 0;
}

int serializable(){
    std::string column_names[] = {"NDB_No",  "Shrt_Desc", "Water", "Energ_Kcal", "Protein", "Lipid_Tot", "Ash", "Carbohydrate", "Fiber_TD", "Sugar_Tot", "Calcium", "Iron", "Magnesium", "Phosphorus", "Potassium", "Sodium", "Zinc", "Copper", "Manganese", "Selenium", "Vit_C", "Thiamin", "Riboflavin", "Niacin", "Panto_acid", "Vit_B", "Folate_Tot", "Folic_acid", "Food_Folate",  "Folate_DFE", "Choline_Tot", "Vit_B12",  "Vit_A_IU", "Vit_A_RAE", "Retinol", "Alpha_Carot", "Beta_Carot", "Beta_Crypt", "Lycopene", "Lut+Zea", "Vit_E", "Vit_D_mcg", "Vit_D_IU", "Vit_K", "FA_Sat", "FA_Mono", "FA_Poly", "Cholestrl", "GmWt_1", "GmWt_Desc1", "GmWt_2", "GmWt_Desc2","Refuse_Pct", "Group_Code"};

    std::string col_types[] = {typeid(int).name(), typeid(std::string).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(),
        typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(double).name(), typeid(std::string).name(), typeid(double).name(), typeid(std::string).name(),
        typeid(short).name(), typeid(int).name()};
    
    //Build map of food description codes:
    std::map<int, int> codes;
    std::ifstream fooddes;
    fooddes.open("/Users/azza/Downloads/sr23abbr/FOOD_DES.txt");
    if(fooddes.is_open()){
        std::string l;
        while(std::getline(fooddes,l)){
            int key;
            int value;
            char *tokline = (char *)malloc((strlen(l.c_str()) + 1)*sizeof(char));
            tokline = strcpy(tokline, l.c_str());
            
            char *token = strtok(tokline, "^~");
            key = atoi(token);
            
            token = strtok(NULL, "^~");
            value = atoi(token);
            
            codes[key] = value;
            
            token = NULL;
            
            free(tokline);
        }
        fooddes.close();
    }
    
    size_t cols = 54;
    size_t size = 7636;
    
    std::vector<std::string> names(54);
    std::vector<std::string> types(54);
    for(int i = 0; i < cols; i++){
        names[i] = column_names[i];
        types[i] = col_types[i];
    }
    
    
    Schema* s = new Schema("fooddb", size, cols, names, types, false);
    
    
    void* colptrs[cols];
    for(int i = 0; i < cols; i++){
        //switch (types[i]){
        if(types[i] == typeid(std::string).name()) colptrs[i] = new std::string[size];
        else if(types[i] == typeid(int).name()) colptrs[i] = new int[size];
        else if(types[i] == typeid(short).name()) colptrs[i] = new short[size];
        else if(types[i] == typeid(double).name()) colptrs[i] = new double[size];
    }
        
    boost::dynamic_bitset<> mmaps[cols];
    for(int i = 0; i < cols; i++){
        mmaps[i].resize(size);
    }
    
    std::string line;
    std::ifstream abbrv;
    abbrv.open("/Users/azza/Downloads/sr23abbr/ABBREV.txt", std::ios::in);
    if (abbrv.is_open()){
        int i = 0;
        while (abbrv.good() && i < size){
            std::getline(abbrv,line);
            
            char *tokline = (char *)malloc(strlen(line.c_str())*sizeof(char));
            strcpy(tokline, line.c_str());
            
            char *token;
            token = strsep(&tokline, "^");
            int c = 0;
            while(token && c < (cols - 1)){
                char* tk = strtok(token, "~");
                if(tk == NULL){
                    mmaps[c][i] = 1;
                }
                else{
                    if(types[c] == typeid(std::string).name()) ((std::string*)colptrs[c])[i] = tk;
                    else if(types[c] == typeid(int).name()) ((int *) colptrs[c])[i] = atoi(tk);
                    else if(types[c] == typeid(short).name()) ((short *) colptrs[c])[i] = (short)atoi(tk);
                    else if(types[c] == typeid(double).name()) ((double *) colptrs[c])[i] = atof(tk);
                }
                c++;
                token = strsep(&tokline, "^");
            }
            ((int *)colptrs[c])[i] = codes[((int *)colptrs[0])[i]];
            i++;
        }
        abbrv.close();
    }
    
    for(int i = 0; i < cols; i++){
        if(types[i] == typeid(std::string).name()) s->writeToColumn<std::string>(names[i], (std::string *) colptrs[i], mmaps[i]);
        else if(types[i] == typeid(int).name()) s->writeToColumn<int>(names[i], (int *) colptrs[i], mmaps[i]);
        else if(types[i] == typeid(short).name()) s->writeToColumn<short>(names[i], (short *) colptrs[i], mmaps[i]);
        else if(types[i] == typeid(double).name())s->writeToColumn<double>(names[i], (double *) colptrs[i], mmaps[i]);
    }
    
    boost::shared_ptr<Column<int> > cptr  = s->getColumn<int>(names[0]);
    
    const int* data = cptr->getData();
    for(int i = 0; i < 10; i++){
        std::cout << data[i] << std::endl;
    }
    return 0;
}

int main(){
	
    std::ofstream ofs ("/tmp/test.txt");
    boost::archive::binary_oarchive oa(ofs);
    std::string x = "boys";
    oa & x;
    std::cout << x;
    ofs.close();
    
    std::ifstream ifs("/tmp/test.txt");
    std::string y;
    boost::archive::binary_iarchive ia(ifs);
    ia & y;
    std::cout << y;
    ifs.close();
    
	//serializable();
	mappable();
	
    return read();
}