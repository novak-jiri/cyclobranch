#!/bin/bash
name="$@"
FileConverter -in "$name" -out "$name"".mzML"
