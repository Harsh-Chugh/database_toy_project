# Database \- Toy project

# Overview

To design a database that supports operations like creating tables, inserting, fetching, deleting rows into tables. 

# Design

Effectively we need to define following classes/structs:

## Database

\`\`\`  
Class Database {  
vector\<Table\> tables  
};  
\`\`\`

## Table

\`\`\`  
Class Table {  
String name;  
Schema schema;  
vector\<Row\> rows;  
};  
\`\`\`

## Column

\`\`\`  
Struct Column{  
String name;  
DataType type;  
}  
\`\`\`  
Where, DataType is:   
\`\`\`  
Enum DataType {  
INT,   
STRING  
}  
\`\`\`

## Schema

\`\`\`  
Class Schema{  
vector\<Column\> columns;  
};  
\`\`\`

## Row

\`\`\`  
Class Row{  
vector\<Value\> values;  
};  
\`\`\`  
Where, Value \= std::variant\<int, string\>

