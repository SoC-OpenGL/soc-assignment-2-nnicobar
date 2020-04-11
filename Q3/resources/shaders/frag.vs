#version 330 core
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float     shininess;
};

struct PointLight
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct Direction
{
    vec3 dir;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
};


in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight point;
uniform Direction direction;
uniform float blinn;
uniform float db;

vec3 GetPointResult( PointLight point, vec3 norm, vec3 viewDir)
{
    // Ambient
    vec3 ambient = point.ambient * vec3(texture(material.diffuse, TexCoords));
    
    //Diffuse
    vec3 lightDir = normalize(point.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = point.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    // Specular
    float spec = 0.0;
    if(blinn>0)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = point.specular * spec * vec3(texture(material.specular, TexCoords));
    
    // Attenuation
    float distance    = length(point.position - FragPos);
    float attenuation = 1.0f / (point.constant + point.linear * distance + point.quadratic * (distance * distance));
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
    
}


vec3 GetDirectionalResult( Direction direction, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-direction.dir);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    float spec = 0.0f;
    if(blinn>0)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    // combine results
    vec3 ambient  = direction.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = direction.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = direction.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}


void main()
{
    
    vec3 finalcolor = vec3(0.0f, 0.0f, 0.0f);
    
    
    //Normal
    vec3 norm = vec3(texture(material.normal, TexCoords));
    norm = normalize(norm * 2.0 - 1.0);
    norm = normalize(TBN * norm);
    
    //ViewDir
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 pointresult = GetPointResult(point, norm, viewDir);
    vec3 directionalresult = GetDirectionalResult(direction, norm, viewDir);
    
    if(db>0)
    {
    finalcolor = directionalresult;
    }
    else
    {
        finalcolor = pointresult;
    }
    
    color = vec4(finalcolor, 1.0f);
}


