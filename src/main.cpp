#define FOV glm::radians(90.0f) // Field of view is 90 degrees
#define ASPECT_RATIO (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT
#define BIAS 0.01f
#define MAX_RECURSION_DEPTH 2
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "skybox.h"
#include "light.h"
#include "color.h"
#include "object.h"
#include "sphere.h"
#include "cube.h"
#include "camera.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

SDL_Renderer *renderer = nullptr;
Light light(glm::vec3(0.0f, 12.0f, -60.0f), 2.7f, Color(255, 255, 255));
Camera camera(glm::vec3(-15.0f, 14.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
Skybox skybox("./textures/skybox.jpg");

float castShadow(const glm::vec3 &shadowOrig, const glm::vec3 &lightDir,
                 const std::vector<Object *> &objects, Object *hitObject)
{
  for (auto &obj : objects)
  {
    if (obj != hitObject)
    {
      glm::vec3 biasedOrigin = shadowOrig + BIAS * lightDir;
      Intersect shadowIntersect = obj->rayIntersect(biasedOrigin, lightDir);
      if (shadowIntersect.isIntersecting && shadowIntersect.distance > 0)
      {
        float lightDistance = glm::length(light.position - shadowOrig);
        float shadowFactor = shadowIntersect.distance / lightDistance;
        shadowFactor = glm::clamp(shadowFactor, 0.0f, 1.0f);
        const float shadowIntensity = 1.0f - shadowFactor;
        return shadowIntensity;
      }
    }
  }
  return 1.0f;
}

Color castRay(const glm::vec3 &orig, const glm::vec3 &dir,
              const std::vector<Object *> &objects, const short recursion = 0)
{
  Intersect closestIntersect;
  Object *hitObject = nullptr;
  float closestDistance = std::numeric_limits<float>::infinity();

  // Buscar la intersección más cercana
  for (auto &obj : objects)
  {
    Intersect intersect = obj->rayIntersect(orig, dir);
    if (intersect.isIntersecting && intersect.distance < closestDistance)
    {
      closestDistance = intersect.distance;
      closestIntersect = intersect;
      hitObject = obj;
    }
  }

  // Si no hay intersección o se alcanza la profundidad máxima de recursión, devolver el color del cielo
  if (!closestIntersect.isIntersecting || recursion >= MAX_RECURSION_DEPTH)
  {
    return skybox.getColor(dir);
  }

  // Obtener el material del objeto golpeado
  const Material &mat = hitObject->getMaterial();

  // Calcular la dirección de la luz y la vista
  glm::vec3 lightDir = glm::normalize(light.position - closestIntersect.point);
  glm::vec3 viewDir = glm::normalize(orig - closestIntersect.point);

  // Calcular la intensidad de la sombra
  float shadowIntensity = castShadow(closestIntersect.point + BIAS * closestIntersect.normal, lightDir, objects, hitObject);
  float intensity = shadowIntensity * light.intensity;

  // Calcular los componentes difusos y especulares
  float diffIntensity = std::max(0.0f, glm::dot(closestIntersect.normal, lightDir));
  glm::vec3 reflectDir = glm::reflect(-lightDir, closestIntersect.normal);
  float specIntensity = std::pow(std::max(0.0f, glm::dot(viewDir, reflectDir)), mat.specularCoefficient);

  // Calcular los colores difusos y especulares
  Color diffuse = diffIntensity * mat.albedo * mat.diffuse;
  Color specular = specIntensity * mat.specularAlbedo * light.color;

  // Calcular los componentes reflejados y refractados, si son aplicables
  Color reflected, refracted;
  if (mat.reflectivity > 0)
  {
    reflected = mat.reflectivity * castRay(closestIntersect.point + BIAS * closestIntersect.normal, reflectDir, objects, recursion + 1);
  }
  if (mat.transparency > 0)
  {
    glm::vec3 refractDir = glm::refract(dir, closestIntersect.normal, mat.refractionIndex);
    refracted = mat.transparency * castRay(closestIntersect.point - BIAS * closestIntersect.normal, refractDir, objects, recursion + 1);
  }

  // Combinar los colores difusos, especulares, reflejados y refractados para obtener el color final
  return (1 - mat.reflectivity - mat.transparency) * (diffuse + specular) + reflected + refracted;
}

void pixel(glm::vec2 position, Color color)
{
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawPoint(renderer, position.x, position.y);
}

void render(std::vector<Object *> &objects)
{
  // This is the "simulated" up vector
  glm::vec3 simulatedUp = glm::vec3(0, 1, 0);

  for (int y = 0; y < SCREEN_HEIGHT; ++y)
  {
    for (int x = 0; x < SCREEN_WIDTH; ++x)
    {
      float screenX = (2.0f * x) / SCREEN_WIDTH - 1.0f;
      float screenY = -(2.0f * y) / SCREEN_HEIGHT + 1.0f;
      screenX *= ASPECT_RATIO;
      glm::vec3 dir = glm::normalize(camera.target - camera.position);
      glm::vec3 right = glm::normalize(glm::cross(dir, simulatedUp));
      glm::vec3 up = glm::cross(right, dir);
      dir = dir + right * screenX + up * screenY;
      Color pixelColor = castRay(camera.position, glm::normalize(dir), objects);
      pixel(glm::vec2(x, y), pixelColor);
    }
  }
}

int main(int argc, char *args[])
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow(
      "Minecraft",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH, SCREEN_HEIGHT,
      SDL_WINDOW_OPENGL);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  bool isRunning = true;
  SDL_Event event;
  float rotateAngle = 0.0f;

  unsigned int lastTime = SDL_GetTicks();
  unsigned int currentTime;
  float dT;

  // Material para la tierra/sabana
  Material tierra(
      Color(150, 75, 0), // Marrón
      1.0f,              // Albedo moderado
      0.3f,              // Especular albedo bajo
      10.0f,             // Coeficiente especular bajo
      0.1f               // Reflectividad baja
  );

  // Material para el agua del lago
  Material agua(
      Color(28, 107, 160), // Azul claro
      0.5f,                // Albedo moderado
      0.8f,                // Alto specular albedo
      50.0f,               // Specular coeficiente alto
      0.5f,                // Reflectividad alta
      0.8f,                // Transparencia alta
      1.33f                // Índice de refracción del agua
  );

  // Material para el sol
  Material solMaterial(
      Color(255, 215, 0), // Amarillo
      1.0f,               // Albedo alto
      0.0f,               // Sin specular albedo
      0.0f,               // Sin specular coeficiente
      0.0f,               // Sin reflectividad
      0.0f,               // Sin transparencia
      0.0f                // Sin índice de refracción
  );

  // Materiales para la casa
  Material casaMuros(
      Color(210, 180, 140), // Color beige o marrón claro
      0.6f,                 // Albedo moderado
      0.3f,                 // Especular albedo bajo
      10.0f,                // Coeficiente especular bajo
      0.1f                  // Reflectividad baja
  );

  Material casaTecho(
      Color(180, 90, 20), // Marrón oscuro
      0.5f,               // Albedo moderado
      0.2f,               // Especular albedo bajo
      5.0f,               // Coeficiente especular bajo
      0.1f                // Reflectividad baja
  );

  // Material para el tronco de acacia
  Material troncoAcacia(
      Color(160, 82, 45), // Color grisáceo
      0.6f,               // Albedo moderado
      0.3f,               // Especular albedo bajo
      10.0f,              // Coeficiente especular bajo
      0.1f                // Reflectividad baja
  );

  // Material para las hojas de acacia
  Material hojasAcacia(
      Color(107, 142, 35), // Verde olivo
      0.8f,                // Albedo alto
      0.2f,                // Especular albedo bajo
      10.0f,               // Coeficiente especular bajo
      0.0f                 // Sin reflectividad
  );

  std::vector<Object *> objects;

  // Tamaño de cada cubo
  float cubeSize = 2.0f;

  // Crear la base de tierra
  for (int x = -15; x <= 15; x += cubeSize)
  {
    for (int z = -15; z <= 15; z += cubeSize)
    {
      if (glm::length(glm::vec2(x, z)) > 10)
      { // Evitar colocar tierra donde estará el lago
        objects.push_back(new Cube(
            glm::vec3(x, -2.0f, z),
            glm::vec3(x + cubeSize, 0.0f, z + cubeSize),
            tierra));
      }
    }
  }

  // Parámetros de la casa
  int baseX = 6;       // Posición X inicial de la casa
  int baseZ = 6;       // Posición Z inicial de la casa
  int alturaMuros = 4; // Altura de los muros de la casa
  int anchoCasa = 6;   // Ancho de la casa (en número de cubos)

  // Construir el techo en forma de pirámide
  for (int nivel = 0; nivel <= anchoCasa / 2; ++nivel)
  {
    int y = alturaMuros + nivel * 1; // Altura de cada nivel del techo
    for (int x = baseX + nivel; x <= baseX + anchoCasa - nivel; x += 1)
    {
      for (int z = baseZ + nivel; z <= baseZ + anchoCasa - nivel; z += 1)
      {
        // Colocar los cubos del techo
        objects.push_back(new Cube(
            glm::vec3(x, y, z),
            glm::vec3(x + 1, y + 1, z + 1),
            casaTecho));
      }
    }
  }

  // Crear el lago
  for (int x = -10; x <= 10; x += cubeSize)
  {
    for (int z = -10; z <= 10; z += cubeSize)
    {
      if (glm::length(glm::vec2(x, z)) <= 10)
      { // Forma circular para el lago
        objects.push_back(new Cube(
            glm::vec3(x, -2.0f, z),
            glm::vec3(x + cubeSize, 0.0f, z + cubeSize),
            agua));
      }
    }
  }

  // Muros de la casa
  for (int x = baseX; x <= baseX + 4; x += 2)
  {
    for (int z = baseZ; z <= baseZ + 4; z += 2)
    {
      if (!(x == baseX + 2 && z == baseZ + 2))
      { // Dejar espacio para la puerta
        objects.push_back(new Cube(
            glm::vec3(x, 0, z),
            glm::vec3(x + 2, 4, z + 2),
            casaMuros));
      }
    }
  }

  // Añadir el sol como una esfera flotante
  objects.push_back(
      new Sphere(
          glm::vec3(0.0f, 15.0f, 0.0f), // Posición del sol en el cielo
          2.0f,                         // Tamaño del sol
          solMaterial                   // Material del sol
          ));

  // Posiciones fijas para los árboles de acacia
  std::vector<glm::vec2> posicionesArboles = {
      {4, -6}, {8, 12}, {-10, -8}, {-12, 6}, {6, -10}, {-6, 10}, {12, -12}, {-4, 4}, {10, 8}, {-8, -4}};

  for (auto &posicion : posicionesArboles)
  {
    int xArbol = posicion.x * cubeSize;
    int zArbol = posicion.y * cubeSize;

    // Verificar que los árboles estén dentro de los márgenes de la base de tierra
    if (xArbol >= -15 && xArbol <= 15 && zArbol >= -15 && zArbol <= 15)
    {
      // Tronco del árbol
      for (int y = 0; y < 6; y += 2)
      {
        objects.push_back(new Cube(
            glm::vec3(xArbol, y, zArbol),
            glm::vec3(xArbol + cubeSize, y + cubeSize, zArbol + cubeSize),
            troncoAcacia));
      }

      // Hojas del árbol
      for (int x = xArbol - cubeSize; x <= xArbol + cubeSize; x += cubeSize)
      {
        for (int z = zArbol - cubeSize; z <= zArbol + cubeSize; z += cubeSize)
        {
          if (x != xArbol || z != zArbol)
          { // Evitar colocar hojas directamente sobre el tronco
            objects.push_back(new Cube(
                glm::vec3(x, 6, z),
                glm::vec3(x + cubeSize, 8, z + cubeSize),
                hojasAcacia));
          }
        }
      }
    }
  }

  int frameCount = 0;
  float elapsedTime = 0.0f;

  while (isRunning)
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
        isRunning = false;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_q:
          isRunning = false;
          break;
        case SDLK_UP:
          // Move closer to the target
          camera.move(-1.0f); // You may need to adjust the value as per your needs
          break;
        case SDLK_DOWN:
          // Move away from the target
          camera.move(1.0f); // You may need to adjust the value as per your needs
          break;
        case SDLK_a:
          // Rotate up
          camera.rotate(-1.0f, 0.0f); // You may need to adjust the value as per your needs
          break;
        case SDLK_d:
          // Rotate down
          camera.rotate(1.0f, 0.0f); // You may need to adjust the value as per your needs
          break;
        case SDLK_w:
          // Rotate left
          camera.rotate(0.0f, -1.0f); // You may need to adjust the value as per your needs
          break;
        case SDLK_s:
          // Rotate right
          camera.rotate(0.0f, 1.0f); // You may need to adjust the value as per your needs
          break;
        default:
          break;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    render(objects);

    SDL_RenderPresent(renderer);

    // Calculate the deltaTime
    currentTime = SDL_GetTicks();
    dT = (currentTime - lastTime) / 1000.0f; // Time since last frame in seconds
    lastTime = currentTime;

    frameCount++;
    elapsedTime += dT;
    if (elapsedTime >= 1.0f)
    {
      float fps = static_cast<float>(frameCount) / elapsedTime;
      std::cout << "FPS: " << fps << std::endl;

      frameCount = 0;
      elapsedTime = 0.0f;
    }
  }

  for (Object *object : objects)
  {
    delete object;
  }
  objects.clear();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
