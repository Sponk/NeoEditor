/*
 * ColladaCameraExporter.cpp
 *
 *  Created on: May 17, 2015
 *      Author: wise
 */


#include "UnitTestPCH.h"

#include <assimp/cexport.h>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#ifndef ASSIMP_BUILD_NO_EXPORT

class ColladaExportCamera : public ::testing::Test {
public:

    virtual void SetUp()
    {
        ex = new Assimp::Exporter();
        im = new Assimp::Importer();

    }

    virtual void TearDown()
    {
        delete ex;
        delete im;
    }

protected:


    Assimp::Exporter* ex;
    Assimp::Importer* im;
};

// ------------------------------------------------------------------------------------------------
TEST_F(ColladaExportCamera, testExportCamera)
{
    const char* file = "cameraExp.dae";

    const aiScene* pTest = im->ReadFile(ASSIMP_TEST_MODELS_DIR "/Collada/cameras.dae",0);
    ASSERT_TRUE(pTest!=NULL);
    ASSERT_TRUE(pTest->HasCameras());


    EXPECT_EQ(AI_SUCCESS,ex->Export(pTest,"collada",file));
    const unsigned int origNumCams( pTest->mNumCameras );
    float *origFOV = new float[ origNumCams ];
    float *orifClipPlaneNear = new float[ origNumCams ];
    float *orifClipPlaneFar = new float[ origNumCams ];
    aiString *names = new aiString[ origNumCams ];
    aiVector3D *pos = new aiVector3D[ origNumCams ];
    for (size_t i = 0; i < origNumCams; i++) {
        const aiCamera *orig = pTest->mCameras[ i ];
        origFOV[ i ] = orig->mHorizontalFOV;
        orifClipPlaneNear[ i ] = orig->mClipPlaneNear;
        orifClipPlaneFar[ i ] = orig->mClipPlaneFar;
        names[ i ] = orig->mName;
        pos[ i ] = orig->mPosition;
    }
    const aiScene* imported = im->ReadFile(file,0);

    ASSERT_TRUE(imported!=NULL);

    EXPECT_TRUE( imported->HasCameras() );
    EXPECT_EQ( origNumCams, imported->mNumCameras );

    for(size_t i=0; i< imported->mNumCameras;i++){
        const aiCamera *read = imported->mCameras[ i ];

        EXPECT_TRUE( names[ i ] == read->mName );
        EXPECT_NEAR( origFOV[ i ],read->mHorizontalFOV, 0.0001f );
        EXPECT_FLOAT_EQ( orifClipPlaneNear[ i ], read->mClipPlaneNear);
        EXPECT_FLOAT_EQ( orifClipPlaneFar[ i ], read->mClipPlaneFar);

        EXPECT_FLOAT_EQ( pos[ i ].x,read->mPosition.x);
        EXPECT_FLOAT_EQ( pos[ i ].y,read->mPosition.y);
        EXPECT_FLOAT_EQ( pos[ i ].z,read->mPosition.z);
    }

    delete [] origFOV;
    delete [] orifClipPlaneNear;
    delete [] orifClipPlaneFar;
    delete [] names;
    delete [] pos;

}

#endif


