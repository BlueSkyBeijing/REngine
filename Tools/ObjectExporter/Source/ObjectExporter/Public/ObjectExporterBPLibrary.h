// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObjectExporterBPLibrary.generated.h"

/*
*   Function library class.
*   Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*   When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*   BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*   BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*   DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*               Its lets you name the node using characters not allowed in C++ function names.
*   CompactNodeTitle - the word(s) that appear on the node.
*   Keywords -  the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu.
*               Good example is "Print String" node which you can find also by using keyword "log".
*   Category -  the category your node will be under in the Blueprint drop-down menu.
*
*   For more info on custom blueprint nodes visit documentation:
*   https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UObjectExporterBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Satic Mesh", Keywords = "Export Satic Mesh"), Category = "UObjectExporter")
    static bool ExportStaticMesh(const UStaticMesh* StaticMesh, const FString& FullFilePathName);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Skeletal Mesh", Keywords = "Export Skeletal Mesh"), Category = "UObjectExporter")
    static bool ExportSkeletalMesh(const USkeletalMesh* SkeletalMesh, const FString& FullFilePathName);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Skeleton", Keywords = "Export Skeleton"), Category = "UObjectExporter")
    static bool ExportSkeleton(const USkeleton* Skeleton, const FString& FullFilePathName);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export AnimSequence", Keywords = "Export AnimSequence"), Category = "UObjectExporter")
    static bool ExportAnimSequence(const UAnimSequence* AnimSequence, const FString& FullFilePathName);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Camera", Keywords = "Export Camera"), Category = "UObjectExporter")
    static bool ExportCamera(const UCameraComponent* Camera, const FString& FullFilePathName);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export MaterialInstace", Keywords = "Export MaterialInstace"), Category = "UObjectExporter")
    static bool ExportMaterialInstance(const UMaterialInstance* MaterialInstace, const FString& FullFilePathName);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Map", Keywords = "Export Map"), Category = "UObjectExporter")
    static bool ExportMap(UObject* WorldContextObject, const FString& FullFilePathName);

};
