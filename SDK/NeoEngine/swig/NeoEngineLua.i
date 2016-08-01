%module NeoEngineLua

%rename Neo::Range::end finish;
%rename Neo::NeoGame::end stop;
%rename Neo::Player::end stop;
%rename Neo::Scene::end stop;

%ignore operator float*;
%ignore operator const float*;
%ignore operator Vector3;
%ignore operator unsigned char*;
%ignore operator const unsigned char*;

%include "NeoEngine.i"
