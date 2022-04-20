#include "defs.h"

VitaTextureData* VitaResource::getTextureData() {
    ASSERT(type == TYPE::TEXTURE);
    return texture_data;
}

VitaResource* VitaResource::createFILE(string path, string key_path) {
    VitaResource* ret = new VitaResource(TYPE::FILE);
    ret->setPath(path);
    ret->setKeyPath(key_path);
    return ret;
}

VitaResource* VitaResource::createTexture(string path, string key_path, int a, int b, int c, int d) {
    VitaResource* ret = new VitaResource(TYPE::TEXTURE);
    ret->setPath(path);
    ret->setKeyPath(key_path);
    ret->texture_data = new VitaTextureData(ret->resource_path, ret->key_path, a, b, c, d);
    return ret;
}

void VitaResource::setPath(string path) { 
    resource_path = VITA_PARTITION + path; 
}
void VitaResource::setKeyPath(string _key_path) { 
    key_path = formatPath(_key_path); 
}
