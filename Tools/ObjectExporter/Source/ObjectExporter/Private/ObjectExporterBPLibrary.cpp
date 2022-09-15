// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectExporterBPLibrary.h"
#include "ObjectExporter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/SkeletalMeshActor.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"


#define TEXTURE_PATH "REngine/Texture/"
#define MATERIAL_PATH "REngine/Material/"
#define STATICMESH_PATH "REngine/StaticMesh/"
#define SKELETALMESH_PATH "REngine/SkeletalMesh/"
#define SKELETON_PATH "REngine/SkeletalMesh/Skeleton/"
#define ANIMATION_PATH "REngine/SkeletalMesh/Animation/"

#define JSON_FILE_POSTFIX ".json"
#define STATIC_MESH_BINARY_FILE_POSTFIX ".stm"
#define SKELETAL_MESH_BINARY_FILE_POSTFIX ".skm"
#define SKELETON_BINARY_FILE_POSTFIX ".skt"
#define ANIMSEQUENCE_BINARY_FILE_POSTFIX ".anm"
#define MATERIAL_BINARY_FILE_POSTFIX ".mat"
#define MAP_BINARY_FILE_POSTFIX ".map"

DECLARE_LOG_CATEGORY_CLASS(ObjectExporterBPLibraryLog, Log, All);

UObjectExporterBPLibrary::UObjectExporterBPLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

bool UObjectExporterBPLibrary::ExportStaticMesh(const UStaticMesh* StaticMesh, const FString& FullFilePathName)
{
    FText OutError;
    if (!FFileHelper::IsFilenameValidForSaving(FullFilePathName, OutError))
    {
        UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportStaticMesh: FullFilePathName is not valid. %s"), *OutError.ToString());

        return false;
    }

    if (StaticMesh != nullptr)
    {
        if (FullFilePathName.EndsWith(JSON_FILE_POSTFIX))
        {
            const int32 FileVersion = 1;
            TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
            JsonRootObject->SetNumberField("FileVersion", FileVersion);
            JsonRootObject->SetStringField("MeshName", *StaticMesh->GetName());

            if (StaticMesh->GetRenderData() != nullptr)
            {
                // Vertex format
                TArray<TSharedPtr<FJsonValue>> JsonVertexFormat;
                JsonRootObject->SetArrayField("VertexFormat", JsonVertexFormat);

                // LODs
                JsonRootObject->SetNumberField("LODCount", StaticMesh->GetRenderData()->LODResources.Num());

                int32 LODIndex = 0;
                TArray< TSharedPtr<FJsonValue> > JsonLODDatas;
                for (const FStaticMeshLODResources& CurLOD : StaticMesh->GetRenderData()->LODResources)
                {
                    TSharedRef<FJsonObject> JsonLODSingle = MakeShareable(new FJsonObject);
                    JsonLODSingle->SetNumberField("LOD", LODIndex);

                    // Vertex data
                    TArray<TSharedPtr<FJsonValue>> JsonVertices;
                    const FPositionVertexBuffer& VertexBuffer = CurLOD.VertexBuffers.PositionVertexBuffer;

                    JsonLODSingle->SetNumberField("VertexCount", VertexBuffer.GetNumVertices());

                    for (uint32 iVertex = 0; iVertex < VertexBuffer.GetNumVertices(); iVertex++)
                    {
                        const FVector3f& Position = VertexBuffer.VertexPosition(iVertex);

                        TSharedRef<FJsonObject> JsonVertex = MakeShareable(new FJsonObject);
                        JsonVertex->SetNumberField("x", Position.X);
                        JsonVertex->SetNumberField("y", Position.Y);
                        JsonVertex->SetNumberField("z", Position.Z);

                        JsonVertices.Emplace(MakeShareable(new FJsonValueObject(JsonVertex)));
                    }
                    JsonLODSingle->SetArrayField("Vertices", JsonVertices);

                    // Index data
                    TArray<TSharedPtr<FJsonValue>> JsonIndices;
                    FIndexArrayView Indices = CurLOD.IndexBuffer.GetArrayView();

                    JsonLODSingle->SetNumberField("IndexCount", Indices.Num());

                    for (int32 iIndex = 0; iIndex < Indices.Num(); iIndex++)
                    {
                        TSharedRef<FJsonObject> JsonIndex = MakeShareable(new FJsonObject);
                        JsonIndex->SetNumberField("index", Indices[iIndex]);

                        JsonIndices.Emplace(MakeShareable(new FJsonValueObject(JsonIndex)));
                    }
                    JsonLODSingle->SetArrayField("Indices", JsonIndices);

                    JsonLODDatas.Emplace(MakeShareable(new FJsonValueObject(JsonLODSingle)));

                    LODIndex++;
                }
                JsonRootObject->SetArrayField("LODs", JsonLODDatas);

                FString JsonContent;
                TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonContent, 0);
                if (FJsonSerializer::Serialize(JsonRootObject, JsonWriter))
                {
                    if (FFileHelper::SaveStringToFile(JsonContent, *FullFilePathName))
                    {
                        UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportStaticMesh: success."));

                        return true;
                    }
                }
            }
        }
        else if (FullFilePathName.EndsWith(STATIC_MESH_BINARY_FILE_POSTFIX))
        {
            // Save to binary file
            IFileManager& FileManager = IFileManager::Get();
            FArchive* FileWriter = FileManager.CreateFileWriter(*FullFilePathName);
            if (nullptr == FileWriter)
            {
                UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportStaticMesh: CreateFileWriter failed."));

                return false;
            }

            for (const FStaticMeshLODResources& CurLOD : StaticMesh->GetRenderData()->LODResources)
            {
                // Vertex data
                const FPositionVertexBuffer& PositionVertexBuffer = CurLOD.VertexBuffers.PositionVertexBuffer;
                const FStaticMeshVertexBuffer& StaticMeshVertexBuffer = CurLOD.VertexBuffers.StaticMeshVertexBuffer;
                int32 NumVertices = PositionVertexBuffer.GetNumVertices();

                *FileWriter << NumVertices;

                for (uint32 iVertex = 0; iVertex < PositionVertexBuffer.GetNumVertices(); iVertex++)
                {
                    FVector3f Position = PositionVertexBuffer.VertexPosition(iVertex);
                    FVector4 TangentZ = StaticMeshVertexBuffer.VertexTangentZ(iVertex);
                    FVector Normal = FVector(TangentZ.X, TangentZ.Y, TangentZ.Z) * TangentZ.W;
                    FVector2f UV = StaticMeshVertexBuffer.GetVertexUV(iVertex, 0);

                    *FileWriter << Position;
                    *FileWriter << Normal;
                    *FileWriter << UV;
                }

                // Index data
                FIndexArrayView Indices = CurLOD.IndexBuffer.GetArrayView();
                int32 NumIndices = Indices.Num();

                *FileWriter << NumIndices;

                for (int32 iIndex = 0; iIndex < Indices.Num(); iIndex++)
                {
                    uint16 Index = Indices[iIndex];
                    *FileWriter << Index;
                }

                //now save only lod 0
                break;
            }

            FileWriter->Close();
            delete FileWriter;
            FileWriter = nullptr;
        }
    }

    UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportStaticMesh: failed."));

    return false;
}

bool UObjectExporterBPLibrary::ExportSkeletalMesh(const USkeletalMesh* SkeletalMesh, const FString& FullFilePathName)
{
    FText OutError;
    if (!FFileHelper::IsFilenameValidForSaving(FullFilePathName, OutError))
    {
        UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportSkeletonalMesh: FullFilePathName is not valid. %s"), *OutError.ToString());

        return false;
    }

    if (SkeletalMesh != nullptr)
    {
        if (FullFilePathName.EndsWith(JSON_FILE_POSTFIX))
        {
            const int32 FileVersion = 1;
            TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
            JsonRootObject->SetNumberField("FileVersion", FileVersion);
        }
        else if (FullFilePathName.EndsWith(SKELETAL_MESH_BINARY_FILE_POSTFIX))
        {
            // Save to binary file
            IFileManager& FileManager = IFileManager::Get();
            FArchive* FileWriter = FileManager.CreateFileWriter(*FullFilePathName);
            if (nullptr == FileWriter)
            {
                UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportSkeletonalMesh: CreateFileWriter failed."));

                return false;
            }

            for (const FSkeletalMeshLODRenderData& CurLOD : SkeletalMesh->GetResourceForRendering()->LODRenderData)
            {
                // Vertex data
                const FPositionVertexBuffer& PositionVertexBuffer = CurLOD.StaticVertexBuffers.PositionVertexBuffer;
                const FStaticMeshVertexBuffer& StaticMeshVertexBuffer = CurLOD.StaticVertexBuffers.StaticMeshVertexBuffer;
                const TArray<FBoneIndexType>& BoneMap = CurLOD.RenderSections[0].BoneMap;
                TArray<FSkinWeightInfo> WeightInfos;
                CurLOD.SkinWeightVertexBuffer.GetSkinWeights(WeightInfos);

                int32 NumVertices = PositionVertexBuffer.GetNumVertices();

                *FileWriter << NumVertices;

                for (uint32 iVertex = 0; iVertex < PositionVertexBuffer.GetNumVertices(); iVertex++)
                {
                    FVector3f Position = PositionVertexBuffer.VertexPosition(iVertex);
                    FVector4 TangentZ = StaticMeshVertexBuffer.VertexTangentZ(iVertex);
                    FVector Normal = FVector(TangentZ.X, TangentZ.Y, TangentZ.Z);
                    FVector2f UV = StaticMeshVertexBuffer.GetVertexUV(iVertex, 0);

                    *FileWriter << Position;
                    *FileWriter << Normal;
                    *FileWriter << UV;

                    FBoneIndexType BoneIndex0 = BoneMap[WeightInfos[iVertex].InfluenceBones[0]];
                    *FileWriter << BoneIndex0;
                    FBoneIndexType BoneIndex1 = BoneMap[WeightInfos[iVertex].InfluenceBones[1]];
                    *FileWriter << BoneIndex1;
                    FBoneIndexType BoneIndex2 = BoneMap[WeightInfos[iVertex].InfluenceBones[2]];
                    *FileWriter << BoneIndex2;
                    FBoneIndexType BoneIndex3 = BoneMap[WeightInfos[iVertex].InfluenceBones[3]];
                    *FileWriter << BoneIndex3;

                    float BoneWeight0 = WeightInfos[iVertex].InfluenceWeights[0] / 255.0f;
                    *FileWriter << BoneWeight0;
                    float BoneWeight1 = WeightInfos[iVertex].InfluenceWeights[1] / 255.0f;
                    *FileWriter << BoneWeight1;
                    float BoneWeight2 = WeightInfos[iVertex].InfluenceWeights[2] / 255.0f;
                    *FileWriter << BoneWeight2;
                    float BoneWeight3 = WeightInfos[iVertex].InfluenceWeights[3] / 255.0f;
                    *FileWriter << BoneWeight3;

                }

                // Index data
                TArray<uint32> Indices;
                CurLOD.MultiSizeIndexContainer.GetIndexBuffer(Indices);
                int32 NumIndices = Indices.Num();

                *FileWriter << NumIndices;

                for (int32 iIndex = 0; iIndex < Indices.Num(); iIndex++)
                {
                    uint16 Index = Indices[iIndex];
                    *FileWriter << Index;
                }
                
                auto ResourceFullName = SkeletalMesh->GetSkeleton()->GetPathName();

                FString ResourcePath, ResourceName;
                ResourceFullName.Split(FString("."), &ResourcePath, &ResourceName);

                *FileWriter << ResourceName;

                //now save only lod 0
                break;
            }

            FileWriter->Close();
            delete FileWriter;
            FileWriter = nullptr;

        }
    }

    UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportStaticMesh: failed."));

    return false;

}

bool UObjectExporterBPLibrary::ExportSkeleton(const USkeleton* Skeleton, const FString& FullFilePathName)
{
    FText OutError;
    if (!FFileHelper::IsFilenameValidForSaving(FullFilePathName, OutError))
    {
        UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportSkeleton: FullFilePathName is not valid. %s"), *OutError.ToString());

        return false;
    }

    if (Skeleton != nullptr)
    {
        if (FullFilePathName.EndsWith(JSON_FILE_POSTFIX))
        {
            const int32 FileVersion = 1;
            TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
            JsonRootObject->SetNumberField("FileVersion", FileVersion);
        }
        else if (FullFilePathName.EndsWith(SKELETON_BINARY_FILE_POSTFIX))
        {
            // Save to binary file
            IFileManager& FileManager = IFileManager::Get();
            FArchive* FileWriter = FileManager.CreateFileWriter(*FullFilePathName);
            if (nullptr == FileWriter)
            {
                UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportSkeleton: CreateFileWriter failed."));

                return false;
            }

            const TArray<FMeshBoneInfo>& BoneInfos = Skeleton->GetReferenceSkeleton().GetRawRefBoneInfo();
            const TArray<FTransform>& BonePose = Skeleton->GetReferenceSkeleton().GetRawRefBonePose();

            int32 NumBoneInfos = BoneInfos.Num();
            int32 NumPosBones = BonePose.Num();

            *FileWriter << NumBoneInfos;
            for (FMeshBoneInfo Boneinfo : BoneInfos)
            {
                *FileWriter << Boneinfo.Name;
                *FileWriter << Boneinfo.ParentIndex;
            }

            *FileWriter << NumPosBones;
            for (FTransform BoneTransform : BonePose)
            {
                FQuat Rot = BoneTransform.GetRotation();
                FVector Tran = BoneTransform.GetTranslation();
                FVector Scale = BoneTransform.GetScale3D();
                *FileWriter << Rot;
                *FileWriter << Tran;
                *FileWriter << Scale;
            }

            FileWriter->Close();
            delete FileWriter;
            FileWriter = nullptr;

        }
    }

    UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportStaticMesh: failed."));

    return false;

}


bool UObjectExporterBPLibrary::ExportAnimSequence(const UAnimSequence* AnimSequence, const FString& FullFilePathName)
{
    FText OutError;
    if (!FFileHelper::IsFilenameValidForSaving(FullFilePathName, OutError))
    {
        UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportAnimSequence: FullFilePathName is not valid. %s"), *OutError.ToString());

        return false;
    }

    if (AnimSequence != nullptr)
    {
        if (FullFilePathName.EndsWith(JSON_FILE_POSTFIX))
        {
            const int32 FileVersion = 1;
            TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
            JsonRootObject->SetNumberField("FileVersion", FileVersion);
        }
        else if (FullFilePathName.EndsWith(ANIMSEQUENCE_BINARY_FILE_POSTFIX))
        {
            // Save to binary file
            IFileManager& FileManager = IFileManager::Get();
            FArchive* FileWriter = FileManager.CreateFileWriter(*FullFilePathName);
            if (nullptr == FileWriter)
            {
                UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportAnimSequence: CreateFileWriter failed."));

                return false;
            }
     
            const TArray<FRawAnimSequenceTrack>& AnimationData = AnimSequence->GetRawAnimationData();
            const TArray<FTrackToSkeletonMap>& TrackToSkeMap = AnimSequence->GetRawTrackToSkeletonMapTable();

            int32 NumberOfFrames = AnimSequence->GetNumberOfSampledKeys();
            *FileWriter << NumberOfFrames;

            float SequenceLength = AnimSequence->GetPlayLength();
            *FileWriter << SequenceLength;

            int32 TrackIndex = 0;
            for (FRawAnimSequenceTrack SequenceTrack : AnimationData)
            {
                int32 BoneIndex = TrackToSkeMap[TrackIndex].BoneTreeIndex;
                *FileWriter << BoneIndex;

                *FileWriter << SequenceTrack.PosKeys;
                *FileWriter << SequenceTrack.RotKeys;
                *FileWriter << SequenceTrack.ScaleKeys;

                TrackIndex++;
            }

            FileWriter->Close();
            delete FileWriter;
            FileWriter = nullptr;

        }
    }

    UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportStaticMesh: failed."));

    return false;

}


bool UObjectExporterBPLibrary::ExportCamera(const UCameraComponent* Camera, const FString& FullFilePathName)
{
    FText OutError;
    if (!FFileHelper::IsFilenameValidForSaving(FullFilePathName, OutError))
    {
        UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportCamera: FullFilePathName is not valid. %s"), *OutError.ToString());

        return false;
    }

    if (Camera != nullptr)
    {
        const int32 FileVersion = 1;
        TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
        JsonRootObject->SetNumberField("FileVersion", FileVersion);

        TSharedRef<FJsonObject> JsonCamera = MakeShareable(new FJsonObject);

        const FVector Position = Camera->GetComponentLocation();
        TSharedRef<FJsonObject> JsonPosition = MakeShareable(new FJsonObject);
        JsonPosition->SetNumberField("x", Position.X);
        JsonPosition->SetNumberField("y", Position.Y);
        JsonPosition->SetNumberField("z", Position.Z);
        JsonCamera->SetObjectField("Location", JsonPosition);

        const FRotator Rotation = Camera->GetComponentRotation();
        TSharedRef<FJsonObject> JsonRotation = MakeShareable(new FJsonObject);
        JsonRotation->SetNumberField("roll", Rotation.Roll);
        JsonRotation->SetNumberField("yaw", Rotation.Yaw);
        JsonRotation->SetNumberField("pitch", Rotation.Pitch);
        JsonCamera->SetObjectField("Rotation", JsonRotation);

        JsonCamera->SetNumberField("FOV", Camera->FieldOfView);
        JsonCamera->SetNumberField("AspectRatio", Camera->AspectRatio);

        JsonRootObject->SetObjectField("Camera", JsonCamera);

        FString JsonContent;
        TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonContent, 0);
        if (FJsonSerializer::Serialize(JsonRootObject, JsonWriter))
        {
            if (FFileHelper::SaveStringToFile(JsonContent, *FullFilePathName))
            {
                UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportCamera: success."));

                return true;
            }
        }
    }

    UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportCamera: failed."));

    return false;
}

bool UObjectExporterBPLibrary::ExportMaterialInstance(const UMaterialInstance* MaterialInstace, const FString& FullFilePathName)
{
    FText OutError;
    if (!FFileHelper::IsFilenameValidForSaving(FullFilePathName, OutError))
    {
        UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportSkeletonalMesh: FullFilePathName is not valid. %s"), *OutError.ToString());

        return false;
    }

    if (MaterialInstace != nullptr)
    {
        if (FullFilePathName.EndsWith(JSON_FILE_POSTFIX))
        {
            const int32 FileVersion = 1;
            TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
            JsonRootObject->SetNumberField("FileVersion", FileVersion);
        }
        else if (FullFilePathName.EndsWith(MATERIAL_BINARY_FILE_POSTFIX))
        {
            // Save to binary file
            IFileManager& FileManager = IFileManager::Get();
            FArchive* FileWriter = FileManager.CreateFileWriter(*FullFilePathName);
            if (nullptr == FileWriter)
            {
                UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportMaterialInstance: CreateFileWriter failed."));

                return false;
            }

            FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
            TArray<FMaterialParameterInfo> OutTextureParameterInfo;
            TArray<FGuid> GuidsTexture;
            MaterialInstace->GetAllTextureParameterInfo(OutTextureParameterInfo, GuidsTexture);
            int32 BlendMode = (int32)MaterialInstace->BlendMode;
            *FileWriter << BlendMode;
            for (const FMaterialParameterInfo& ParameterInfo : OutTextureParameterInfo)
            {
                UTexture* Texture = nullptr;
                MaterialInstace->GetTextureParameterValue(ParameterInfo, Texture);

                if (Texture != nullptr)
                {
                    FString ResourceFullName = Texture->GetPathName();
                    FString ResourcePath, ResourceName;
                    ResourceFullName.Split(FString("."), &ResourcePath, &ResourceName);

                    *FileWriter << ResourceName;

                    FString TempSavePath = FPaths::ProjectIntermediateDir();
                    FString SavePath = FPaths::ProjectSavedDir() + TEXTURE_PATH;
                    TArray<UObject*> ObjectsToExport;
                    ObjectsToExport.Add(Texture);
                    AssetToolsModule.Get().ExportAssets(ObjectsToExport, *TempSavePath);
                    // Cmd dir only \\ work
                    IFileManager::Get().MakeDirectory(*SavePath);
                    FString CmdString = FPaths::ProjectPluginsDir() + "ObjectExporter/texconv.exe -ft dds " + FPaths::ProjectIntermediateDir() + ResourcePath + ".TGA" + " -o " + SavePath;
                    CmdString = CmdString.Replace(TEXT("/"), TEXT("\\"));
                    system(TCHAR_TO_ANSI(*CmdString));
                }
            }

            EMaterialShadingModel MaterialShadingModel = MaterialInstace->GetShadingModels().GetFirstShadingModel();
            int32 ShadingModel = (int32)MaterialShadingModel;
            *FileWriter << ShadingModel;

            TArray<FMaterialParameterInfo> OutScalarParameterInfo;
            TArray<FGuid> GuidsScalar;
            MaterialInstace->GetAllScalarParameterInfo(OutScalarParameterInfo, GuidsScalar);
            for (const FMaterialParameterInfo& ParameterInfo : OutScalarParameterInfo)
            {
                float Opacity = 1.0f;
                if (MaterialInstace->GetScalarParameterValue(ParameterInfo, Opacity))
                {
                    *FileWriter << Opacity;
                }
            }

            FileWriter->Close();
            delete FileWriter;
            FileWriter = nullptr;

        }
    }

    UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportStaticMesh: failed."));

    return false;
}

bool UObjectExporterBPLibrary::ExportMap(UObject* WorldContextObject, const FString& FullFilePathName)
{
    if (!IsValid(WorldContextObject) || !IsValid(WorldContextObject->GetWorld()))
    {
        return false;
    }

    if (FullFilePathName.EndsWith(MAP_BINARY_FILE_POSTFIX))
    {
        // Save to binary file
        IFileManager& FileManager = IFileManager::Get();
        FArchive* FileWriter = FileManager.CreateFileWriter(*FullFilePathName);
        if (nullptr == FileWriter)
        {
            UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportMap: CreateFileWriter failed."));

            return false;
        }

        UWorld* World = WorldContextObject->GetWorld();

        TArray<AActor*> AllCameraActors;
        UGameplayStatics::GetAllActorsOfClass(World, ACameraActor::StaticClass(), AllCameraActors);
        int32 CameraCount = AllCameraActors.Num();

        *FileWriter << CameraCount;

        for (AActor* Actor : AllCameraActors)
        {
            UCameraComponent* Component = Cast<UCameraComponent>(Actor->GetComponentByClass(UCameraComponent::StaticClass()));
            check(Component != nullptr);
            auto Transform = Component->GetComponentToWorld();
            auto Location = Transform.GetLocation();
            auto Rotation = Transform.GetRotation();
            auto Rotator = Rotation.Rotator();
            auto Direction = Rotation.Vector();
            auto Target = Location + Direction * 100.0f;
            auto FOV = Component->FieldOfView;
            auto AspectRatio = Component->AspectRatio;

            *FileWriter << Location;
            *FileWriter << Target;
            *FileWriter << FOV;
            *FileWriter << AspectRatio;
        }

        TArray<AActor*> AllDirectionalLightActors;
        UGameplayStatics::GetAllActorsOfClass(World, ADirectionalLight::StaticClass(), AllDirectionalLightActors);
        int32 DirectionalLightCount = AllDirectionalLightActors.Num();

        *FileWriter << DirectionalLightCount;

        for (AActor* Actor : AllDirectionalLightActors)
        {
            UDirectionalLightComponent* Component = Cast<UDirectionalLightComponent>(Actor->GetComponentByClass(UDirectionalLightComponent::StaticClass()));
            check(Component != nullptr);
            auto Transform = Component->GetComponentToWorld();
            auto Location = Transform.GetLocation();
            auto Rotation = Transform.GetRotation();
            auto Rotator = Rotation.Rotator();
            auto Direction = Rotation.Vector();
            auto Color = FLinearColor::FromSRGBColor(Component->LightColor);
            auto Intensity = Component->Intensity;

            *FileWriter << Color;
            *FileWriter << Direction;
            *FileWriter << Intensity;
        }

        TArray<AActor*> AllPointLightActors;
        UGameplayStatics::GetAllActorsOfClass(World, APointLight::StaticClass(), AllPointLightActors);
        int32 PointLightCount = AllPointLightActors.Num();

        *FileWriter << PointLightCount;

        for (AActor* Actor : AllPointLightActors)
        {
            UPointLightComponent* Component = Cast<UPointLightComponent>(Actor->GetComponentByClass(UPointLightComponent::StaticClass()));
            check(Component != nullptr);
            auto Transform = Component->GetComponentToWorld();
            auto Location = Transform.GetLocation();
            auto AttenuationRadius = Component->AttenuationRadius;
            auto LightFalloffExponent = Component->LightFalloffExponent;
            auto Color = FLinearColor::FromSRGBColor(Component->LightColor);
            auto Intensity = Component->Intensity;

            *FileWriter << Color;
            *FileWriter << Location;
            *FileWriter << Intensity;
            *FileWriter << AttenuationRadius;
            *FileWriter << LightFalloffExponent;
        }


        TArray<AActor*> AllStaticMeshActors;
        UGameplayStatics::GetAllActorsOfClass(World, AStaticMeshActor::StaticClass(), AllStaticMeshActors);
        int32 StaticMeshActorCount = AllStaticMeshActors.Num();

        *FileWriter << StaticMeshActorCount;

        for (AActor* Actor : AllStaticMeshActors)
        {
            UStaticMeshComponent* Component = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
            check(Component != nullptr);
            auto Transform = Component->GetComponentToWorld();
            auto Location = Transform.GetLocation();
            auto Rotation = Transform.GetRotation();
            auto Rotator = Rotation.Rotator();
            auto Direction = Rotation.Vector();
            auto ResourceFullName = Component->GetStaticMesh()->GetPathName();
            auto MaterialFullName = Component->GetMaterial(0)->GetPathName();

            FString ResourcePath, ResourceName;
            ResourceFullName.Split(FString("."), &ResourcePath, &ResourceName);

            FString MaterialPath, MaterialName;
            MaterialFullName.Split(FString("."), &MaterialPath, &MaterialName);

            *FileWriter << Rotation;
            *FileWriter << Location;
            *FileWriter << ResourceName;
            *FileWriter << MaterialName;

            FString SaveStaticMeshPath = FPaths::ProjectSavedDir() + STATICMESH_PATH + ResourceName + STATIC_MESH_BINARY_FILE_POSTFIX;
            ExportStaticMesh(Component->GetStaticMesh(), SaveStaticMeshPath);

            TArray<UMaterialInterface*> Materials = Component->GetMaterials();

            for (UMaterialInterface* Material : Materials)
            {
                UMaterialInstance* Instance = Cast<UMaterialInstance>(Material);
                if (Instance->IsValidLowLevel())
                {
                    FString SaveMaterialPath = FPaths::ProjectSavedDir() + MATERIAL_PATH + MaterialName + MATERIAL_BINARY_FILE_POSTFIX;

                    ExportMaterialInstance(Instance, SaveMaterialPath);
                }
            }
                
        }

        TArray<AActor*> AllSkeletalMeshActors;
        UGameplayStatics::GetAllActorsOfClass(World, ASkeletalMeshActor::StaticClass(), AllSkeletalMeshActors);
        int32 SkeletalMeshActorCount = AllSkeletalMeshActors.Num();

        *FileWriter << SkeletalMeshActorCount;

        for (AActor* Actor : AllSkeletalMeshActors)
        {
            USkeletalMeshComponent* Component = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
            check(Component != nullptr);
            auto Transform = Component->GetComponentToWorld();
            auto Location = Transform.GetLocation();
            auto Rotation = Transform.GetRotation();
            auto Rotator = Rotation.Rotator();
            auto Direction = Rotation.Vector();
            auto ResourceFullName = Component->SkeletalMesh->GetPathName();
            auto AnimationFullName = Component->AnimationData.AnimToPlay->GetPathName();
            auto MaterialFullName = Component->GetMaterial(0)->GetPathName();

            FString ResourcePath, ResourceName;
            ResourceFullName.Split(FString("."), &ResourcePath, &ResourceName);

            FString AnimationPath, AnimationName;
            AnimationFullName.Split(FString("."), &AnimationPath, &AnimationName);

            FString MaterialPath, MaterialName;
            MaterialFullName.Split(FString("."), &MaterialPath, &MaterialName);

            *FileWriter << Rotation;
            *FileWriter << Location;
            *FileWriter << ResourceName;
            *FileWriter << AnimationName;
            *FileWriter << MaterialName;

            FString SaveSkeletalMeshPath = FPaths::ProjectSavedDir() + SKELETALMESH_PATH + ResourceName + SKELETAL_MESH_BINARY_FILE_POSTFIX;
            ExportSkeletalMesh(Component->SkeletalMesh, SaveSkeletalMeshPath);

            TArray<UMaterialInterface*> Materials = Component->GetMaterials();

            for (UMaterialInterface* Material : Materials)
            {
                UMaterialInstance* Instance = Cast<UMaterialInstance>(Material);
                if (Instance->IsValidLowLevel())
                {
                    FString SaveMaterialPath = FPaths::ProjectSavedDir() + MATERIAL_PATH + MaterialName + MATERIAL_BINARY_FILE_POSTFIX;

                    ExportMaterialInstance(Instance, SaveMaterialPath);
                }
            }

            auto SkeletonFullName = Component->SkeletalMesh->GetSkeleton()->GetPathName();

            FString SkeletonPath, SkeletonName;
            SkeletonFullName.Split(FString("."), &SkeletonPath, &SkeletonName);

            FString SaveSkeletonPath = FPaths::ProjectSavedDir() + SKELETON_PATH + SkeletonName + SKELETON_BINARY_FILE_POSTFIX;
            ExportSkeleton(Component->SkeletalMesh->GetSkeleton(), SaveSkeletonPath);
 
            FString SaveAnimSequencePath = FPaths::ProjectSavedDir() + ANIMATION_PATH + AnimationName + ANIMSEQUENCE_BINARY_FILE_POSTFIX;
            ExportAnimSequence(Cast<UAnimSequence>(Component->AnimationData.AnimToPlay), SaveAnimSequencePath);
        }


        FileWriter->Close();
        delete FileWriter;
        FileWriter = nullptr;

        UE_LOG(ObjectExporterBPLibraryLog, Log, TEXT("ExportMap: success."));

        return true;

    }

    UE_LOG(ObjectExporterBPLibraryLog, Warning, TEXT("ExportMap: failed."));

    return false;
}
