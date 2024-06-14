#ifndef DEV_CPP_INTERFACE_H
#define DEV_CPP_INTERFACE_H

#include "SFML/Graphics.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include <memory>
#include <vector>
enum sujets { BOOL_TOR, BOOL_FERMETURE, NB_SUJETS };
class Ihm {
  public:
    Ihm(sf::RenderWindow *fenêtre);
    void affichage_ihm();
    void évènements_ihm(sf::Event évènements);
    void *abonnement(enum sujets sujet);

  private:
    sf::RenderWindow *fenêtre;
    sf::Clock horloge{};
    void contenu();
    std::vector<std::unique_ptr<int>> abonnements;
};

#endif // DEV_CPP_INTERFACE_H