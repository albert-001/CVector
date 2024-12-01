# CVector
An efficient C vector implementation, good for embedded C development.

It has the following features,

1. It provides the same functions as the standard vector data structure in C.
2. It uses lazy deletion and conditonal memory compaction, so it is efficient for element deletion.
3. It uses a bitmap internally to track free and used slots, so it is efficient for element searching.
4. It compacts memory when fragment is large so it saves memory space.
