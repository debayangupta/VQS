#ifndef DATATYPES_H_
#define DATATYPES_H_

#define SCHEMAPATH "/vqs/db/schema/"
#define DATAPATH "/vqs/db/data/"

typedef unsigned short FieldType;

#define INT 0
#define FLOAT 1
#define DOUBLE 2
#define SHORT 3
#define STRING 4

typedef char mstr[120];

extern int typesize[];

#endif