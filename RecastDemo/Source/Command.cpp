#include <stdio.h>
#include <vector>
#include <string>

#include "InputGeom.h"
#include "Sample_SoloMesh.h"
#include "Sample_TileMesh.h"

using std::string;
using std::vector;

void command(int argc, char** argv)
{
    vector<string> args(argv + 1, argv + argc);
    float tileSize = 0;
    float cellSize = 0;
    float agentRadius = 0.1f;
    float agentHeight = 2.0f;
    float maxClimb = 0.75f;
    float maxSlop = 60.0f;
    string inputFile;
    string outputFile;

    for (vector<string>::iterator i = args.begin(); i != args.end(); ++i)
    {
        if (*i == "--tileSize")
        {
            tileSize = stof(*++i);
        }
        else if (*i == "--cellSize")
        {
            cellSize = stof(*++i);
        }
        else if (*i == "--agentRadius")
        {
            agentRadius = stof(*++i);
        }
        else if (*i == "--agentHeight")
        {
            agentHeight = stof(*++i);
        }
        else if (*i == "--maxClimb")
        {
            maxClimb = stof(*++i);
        }
        else if (*i == "--maxSlope")
        {
            maxSlope = stof(*++i);
        }
        else if (*i == "--inputFile")
        {
            inputFile = *++i;
        }
        else if (*i == "--outputFile")
        {
            outputFile = *++i;
        }
        else
        {
            printf("RecastDemo --inputFile <F> --outputFile <F> --tileSize %.2f --cellSize %.2f"
                    " --agentRadius %.2f --agentHeight %.2f --maxClimb %.2f --maxSlope %.2f\n",
                    tileSize, cellSize, agentRadius, agentHeight, maxClimb, maxSlope);
            return;
        }
    }

    FILE* fp = fopen(inputFile.c_str(), "rb");
    if (!fp)
    {
        printf("inputFile %s not found\n", inputFile.c_str());
        return;
    }
    fclose(fp);
    if (outputFile == "")
    {
        printf("outputFile is empty\n");
        return;
    }

    BuildContext ctx;
    InputGeom* geom = new InputGeom;
    if (!geom->load(&ctx, inputFile.c_str()))
    {
        delete geom;
        return;
    }

    Sample* sample = NULL;
    if (tileSize <= 1)
    {
        sample = new Sample_SoloMesh();
    }
    else
    {
        sample = new Sample_TileMesh();
    }

    BuildSettings settings;
    memset(&settings, 0, sizeof(settings));

    rcVcopy(settings.navMeshBMin, geom->getNavMeshBoundsMin());
    rcVcopy(settings.navMeshBMax, geom->getNavMeshBoundsMax());

    sample->collectSettings(settings);

    settings.tileSize = tileSize;
    settings.cellSize = cellSize;
    settings.agentHeight = agentHeight;
    settings.agentRadius = agentRadius;
    settings.agentMaxClimb = maxClimb;
    settings.agentMaxSlope = maxSlope;

    geom->setBuildSettings(&settings);

    sample->setContext(&ctx);
    sample->handleMeshChanged(geom);
    sample->handleSettings();

    ctx.resetLog();
    if (sample->handleBuild())
    {
        sample->saveAll(outputFile.c_str());
    }
    for (int i = 0; i < ctx.getLogCount(); ++i)
        print("%s\n", ctx.getLogText(i));

    delete sample;
    delete geom;
    return;
}

void buildSoloMesh(const char* path, const char* output)
{
    FILE* fp = fopen(path, "rb");
    if (!fp) return

    BuildContext ctx;
    InputGeom* geom = new InputGeom;
    if (!geom->load(&ctx, path))
    {
        delete geom;
        return;
    }

    Sample* sample = new Sample_SoloMesh();
    sample->setContext(&ctx);
    sample->handleMeshChanged(geom);

    if (sample->handleBuild())
    {
        sample->saveAll(output);
    }

    delete sample;
    delete geom;
    return;
}
