
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "traffic_descriptor.h"

OpenAPI_traffic_descriptor_t *OpenAPI_traffic_descriptor_create(
    char *dnn,
    OpenAPI_snssai_t *s_nssai,
    OpenAPI_list_t *ddd_traffic_descriptor_list
)
{
    OpenAPI_traffic_descriptor_t *traffic_descriptor_local_var = ogs_malloc(sizeof(OpenAPI_traffic_descriptor_t));
    ogs_assert(traffic_descriptor_local_var);

    traffic_descriptor_local_var->dnn = dnn;
    traffic_descriptor_local_var->s_nssai = s_nssai;
    traffic_descriptor_local_var->ddd_traffic_descriptor_list = ddd_traffic_descriptor_list;

    return traffic_descriptor_local_var;
}

void OpenAPI_traffic_descriptor_free(OpenAPI_traffic_descriptor_t *traffic_descriptor)
{
    OpenAPI_lnode_t *node = NULL;

    if (NULL == traffic_descriptor) {
        return;
    }
    if (traffic_descriptor->dnn) {
        ogs_free(traffic_descriptor->dnn);
        traffic_descriptor->dnn = NULL;
    }
    if (traffic_descriptor->s_nssai) {
        OpenAPI_snssai_free(traffic_descriptor->s_nssai);
        traffic_descriptor->s_nssai = NULL;
    }
    if (traffic_descriptor->ddd_traffic_descriptor_list) {
        OpenAPI_list_for_each(traffic_descriptor->ddd_traffic_descriptor_list, node) {
            OpenAPI_ddd_traffic_descriptor_free(node->data);
        }
        OpenAPI_list_free(traffic_descriptor->ddd_traffic_descriptor_list);
        traffic_descriptor->ddd_traffic_descriptor_list = NULL;
    }
    ogs_free(traffic_descriptor);
}

cJSON *OpenAPI_traffic_descriptor_convertToJSON(OpenAPI_traffic_descriptor_t *traffic_descriptor)
{
    cJSON *item = NULL;
    OpenAPI_lnode_t *node = NULL;

    if (traffic_descriptor == NULL) {
        ogs_error("OpenAPI_traffic_descriptor_convertToJSON() failed [TrafficDescriptor]");
        return NULL;
    }

    item = cJSON_CreateObject();
    if (traffic_descriptor->dnn) {
    if (cJSON_AddStringToObject(item, "dnn", traffic_descriptor->dnn) == NULL) {
        ogs_error("OpenAPI_traffic_descriptor_convertToJSON() failed [dnn]");
        goto end;
    }
    }

    if (traffic_descriptor->s_nssai) {
    cJSON *s_nssai_local_JSON = OpenAPI_snssai_convertToJSON(traffic_descriptor->s_nssai);
    if (s_nssai_local_JSON == NULL) {
        ogs_error("OpenAPI_traffic_descriptor_convertToJSON() failed [s_nssai]");
        goto end;
    }
    cJSON_AddItemToObject(item, "sNssai", s_nssai_local_JSON);
    if (item->child == NULL) {
        ogs_error("OpenAPI_traffic_descriptor_convertToJSON() failed [s_nssai]");
        goto end;
    }
    }

    if (traffic_descriptor->ddd_traffic_descriptor_list) {
    cJSON *ddd_traffic_descriptor_listList = cJSON_AddArrayToObject(item, "dddTrafficDescriptorList");
    if (ddd_traffic_descriptor_listList == NULL) {
        ogs_error("OpenAPI_traffic_descriptor_convertToJSON() failed [ddd_traffic_descriptor_list]");
        goto end;
    }
    OpenAPI_list_for_each(traffic_descriptor->ddd_traffic_descriptor_list, node) {
        cJSON *itemLocal = OpenAPI_ddd_traffic_descriptor_convertToJSON(node->data);
        if (itemLocal == NULL) {
            ogs_error("OpenAPI_traffic_descriptor_convertToJSON() failed [ddd_traffic_descriptor_list]");
            goto end;
        }
        cJSON_AddItemToArray(ddd_traffic_descriptor_listList, itemLocal);
    }
    }

end:
    return item;
}

OpenAPI_traffic_descriptor_t *OpenAPI_traffic_descriptor_parseFromJSON(cJSON *traffic_descriptorJSON)
{
    OpenAPI_traffic_descriptor_t *traffic_descriptor_local_var = NULL;
    OpenAPI_lnode_t *node = NULL;
    cJSON *dnn = NULL;
    cJSON *s_nssai = NULL;
    OpenAPI_snssai_t *s_nssai_local_nonprim = NULL;
    cJSON *ddd_traffic_descriptor_list = NULL;
    OpenAPI_list_t *ddd_traffic_descriptor_listList = NULL;
    dnn = cJSON_GetObjectItemCaseSensitive(traffic_descriptorJSON, "dnn");
    if (dnn) {
    if (!cJSON_IsString(dnn) && !cJSON_IsNull(dnn)) {
        ogs_error("OpenAPI_traffic_descriptor_parseFromJSON() failed [dnn]");
        goto end;
    }
    }

    s_nssai = cJSON_GetObjectItemCaseSensitive(traffic_descriptorJSON, "sNssai");
    if (s_nssai) {
    s_nssai_local_nonprim = OpenAPI_snssai_parseFromJSON(s_nssai);
    if (!s_nssai_local_nonprim) {
        ogs_error("OpenAPI_snssai_parseFromJSON failed [s_nssai]");
        goto end;
    }
    }

    ddd_traffic_descriptor_list = cJSON_GetObjectItemCaseSensitive(traffic_descriptorJSON, "dddTrafficDescriptorList");
    if (ddd_traffic_descriptor_list) {
        cJSON *ddd_traffic_descriptor_list_local = NULL;
        if (!cJSON_IsArray(ddd_traffic_descriptor_list)) {
            ogs_error("OpenAPI_traffic_descriptor_parseFromJSON() failed [ddd_traffic_descriptor_list]");
            goto end;
        }

        ddd_traffic_descriptor_listList = OpenAPI_list_create();

        cJSON_ArrayForEach(ddd_traffic_descriptor_list_local, ddd_traffic_descriptor_list) {
            if (!cJSON_IsObject(ddd_traffic_descriptor_list_local)) {
                ogs_error("OpenAPI_traffic_descriptor_parseFromJSON() failed [ddd_traffic_descriptor_list]");
                goto end;
            }
            OpenAPI_ddd_traffic_descriptor_t *ddd_traffic_descriptor_listItem = OpenAPI_ddd_traffic_descriptor_parseFromJSON(ddd_traffic_descriptor_list_local);
            if (!ddd_traffic_descriptor_listItem) {
                ogs_error("No ddd_traffic_descriptor_listItem");
                goto end;
            }
            OpenAPI_list_add(ddd_traffic_descriptor_listList, ddd_traffic_descriptor_listItem);
        }
    }

    traffic_descriptor_local_var = OpenAPI_traffic_descriptor_create (
        dnn && !cJSON_IsNull(dnn) ? ogs_strdup(dnn->valuestring) : NULL,
        s_nssai ? s_nssai_local_nonprim : NULL,
        ddd_traffic_descriptor_list ? ddd_traffic_descriptor_listList : NULL
    );

    return traffic_descriptor_local_var;
end:
    if (s_nssai_local_nonprim) {
        OpenAPI_snssai_free(s_nssai_local_nonprim);
        s_nssai_local_nonprim = NULL;
    }
    if (ddd_traffic_descriptor_listList) {
        OpenAPI_list_for_each(ddd_traffic_descriptor_listList, node) {
            OpenAPI_ddd_traffic_descriptor_free(node->data);
        }
        OpenAPI_list_free(ddd_traffic_descriptor_listList);
        ddd_traffic_descriptor_listList = NULL;
    }
    return NULL;
}

OpenAPI_traffic_descriptor_t *OpenAPI_traffic_descriptor_copy(OpenAPI_traffic_descriptor_t *dst, OpenAPI_traffic_descriptor_t *src)
{
    cJSON *item = NULL;
    char *content = NULL;

    ogs_assert(src);
    item = OpenAPI_traffic_descriptor_convertToJSON(src);
    if (!item) {
        ogs_error("OpenAPI_traffic_descriptor_convertToJSON() failed");
        return NULL;
    }

    content = cJSON_Print(item);
    cJSON_Delete(item);

    if (!content) {
        ogs_error("cJSON_Print() failed");
        return NULL;
    }

    item = cJSON_Parse(content);
    ogs_free(content);
    if (!item) {
        ogs_error("cJSON_Parse() failed");
        return NULL;
    }

    OpenAPI_traffic_descriptor_free(dst);
    dst = OpenAPI_traffic_descriptor_parseFromJSON(item);
    cJSON_Delete(item);

    return dst;
}

