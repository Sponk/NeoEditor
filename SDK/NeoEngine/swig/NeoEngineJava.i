%module NeoEngineJava

%rename String NeoString;
%ignore Neo::PhysicsConstraint;//::getParentName;

%ignore operator float*;
%ignore operator const float*;
%ignore operator Vector3;
%ignore operator unsigned char*;
%ignore operator const unsigned char*;
%ignore operator +=;
%ignore operator /=;
%ignore operator *=;
%ignore operator -=;
%ignore operator !=;
%ignore operator ();

%rename(add) operator+;
%rename(sub) operator-;
%rename(div) operator/;
%rename(mul) operator*;
%rename(equals) operator==;
%rename(set) operator=;

%include "NeoEngine.i"
