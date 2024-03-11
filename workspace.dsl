workspace {
    name "Социальная сеть"
    description "Система позволяет пользователям создавать профили в социальной сети, добавлять записи на стену, искать других пользователей и общаться с ними."

    # включаем режим с иерархической системой идентификаторов
    !identifiers hierarchical
    
    !docs documentation
    !adrs decisions

    # Модель архитектуры
    model {

        # Настраиваем возможность создания вложенных груп
        properties { 
            structurizr.groupSeparator "/"
        }

        # Описание компонент модели
        user = person "Пользователь социальной сети" {
            tags "Person"
        }
        socnet = softwareSystem "Социальная сеть" {
            description "Позволяет пользователям создавать профили в социальной сети, добавлять записи на стену, искать других пользователей и общаться с ними."

            group "Клиентские приложения" {
                web_app = container "Веб приложение клиента" {
                    description "Позволяет клиенту пользоваться социальной сетью с помощью веб-браузера."
                    technology "Web Browser"
                    tags "web_app"
                }

                mobile_app = container "Мобильное приложение клиента" {
                    description "Позволяет клиенту пользоваться социальной сетью с помощью мобильного приложения"
                    technology "Android/iOS app"
                    tags "mobile_app"
                    
                }
            }

            group "API клиента" {
                web_back = container "Backend веб-приложения клиента" {
                    description "Осуществляет операции, запрашиваемые пользователем."
                    technology "Java and Spring MVC"
                }

                mobile_back = container "Backend мобильного приложения клиента" {
                    description "Осуществляет операции, запрашиваемые пользователем."
                    technology "Java and Spring MVC"
                }
            }

            group "Слой данных" {
                user_database = container "User Database" {
                    description "База данных пользователей"
                    technology "PostgreSQL 15"
                    tags "database"
                }

                wall_database = container "Wall Database" {
                    description "База данных стены пользователя"
                    technology "PostgreSQL 15"
                    tags "database"
                }

                messages_database = container "Messages Database" {
                    description "База данных сообщений (PtP)"
                    technology "PostgreSQL 15"
                    tags "database"
                }
            }

            web_back -> user_database "Получение/изменение данных пользователей" "JDBC"
            web_back -> wall_database "Получение/изменение данных стены пользователя" "JDBC"
            web_back -> messages_database "Получение/изменение данных о сообщениях пользователей" "JDBC"

            mobile_back -> user_database "Получение/изменение данных пользователей" "JDBC"
            mobile_back -> wall_database "Получение/изменение данных стены пользователя" "JDBC"
            mobile_back -> messages_database "Получение/изменение данных о сообщениях пользователей" "JDBC"

            web_app -> web_back "Создание/изменение/получение данных" "REST/HTTP: 80, 443" "container_only"
            mobile_app -> mobile_back "Создание/изменение/получение данных" "REST/HTTP: 80, 443" "container_only"

            user -> web_app "Использование веб-приложения социальной сети" "HTTPS:443" "container_only"
            user -> mobile_app "Использование веб-приложения социальной сети"
        }

        user -> socnet "Использование социальной сети"

        deploymentEnvironment "Production" {
            ios_device = deploymentNode "iOS Device" {
                description "Мобильные телефоны и планшеты клиентов во внешней сети"
                properties {
                    "os" "iOS 12+"
                }
                client_mobile_app_instance1 = containerInstance socnet.mobile_app
            }
            
            android_device = deploymentNode "Android Device" {
                description "Мобильные телефоны и планшеты клиентов во внешней сети"
                properties {
                    "os" "Android 5.0+"
                }
                client_mobile_app_instance2 = containerInstance socnet.mobile_app
            }
            users_device = deploymentNode "User`s Device" {
                description "Стационарные компьютеры пользователей во внешней сети или мобильные устройства с установленным браузером"
                web_browser = deploymentNode "Web Browser" {
                    technology "Chrome, Firefox, Safari, Edge, Yandex"
                    client_web_app_instance = containerInstance socnet.web_app
                }
            }
            
            dmz = deploymentNode "DMZ" {
                fw = infrastructureNode "Firewall" {
                    description "Аппаратный межсетевой экран"
                    technology "Континент 4"
                }

                lb1 = infrastructureNode "LoadBalancer Mobile" {
                    description "Балансировщик запросов к backend мобильного приложения"
                    technology "nginx"
                }

                lb2 = infrastructureNode "LoadBalancer Web" {
                    description "Балансировщик запросов к backend веб-приложения"
                    technology "nginx"
                }

                back_mobile_server = deploymentNode "Mobile Backend Server" {
                    description "Backend для мобильного приложения"
                    technology "Ubuntu 22.04 LTS"
                    instances 4
                    apache_1 = deploymentNode "Apache Tomcat" {
                        client_mobile_app_backend_instance = containerInstance socnet.mobile_back
                        technology "Apache Tomcat 10.x"
                    }
                }

                back_web_server = deploymentNode "Web Backend Server" {
                    description "Backend для веб-приложения"
                    technology "Ubuntu 22.04 LTS"
                    instances 4
                    apache_2 = deploymentNode "Apache Tomcat" {
                        client_web_app_backend_instance = containerInstance socnet.web_back
                        technology "Apache Tomcat 10.x"
                    }
                }

                fw -> lb1 "Балансировка нагрузки" "REST/HTTP: 80, 443"
                fw -> lb2 "Балансировка нагрузки" "REST/HTTP: 80, 443"

                lb1 -> back_mobile_server "REST/HTTP: 80, 443"
                lb2 -> back_web_server "REST/HTTP: 80, 443"

                
            }

            ios_device.client_mobile_app_instance1 -> dmz.fw "REST/HTTP: 80, 443"
            android_device.client_mobile_app_instance2 -> dmz.fw "REST/HTTP: 80, 443"
            users_device.web_browser.client_web_app_instance -> dmz.fw "REST/HTTP: 80, 443"


            db_cluster = deploymentNode "Database Cluster" {
                db_1 = deploymentNode "socnet-db 1" {
                    technology "Ubuntu 22.04 LTS"
                    postgres_prim = deploymentNode "Postgres - Primary" {
                        technology "PostgreSQL 15"
                        containerInstance socnet.user_database
                        containerInstance socnet.wall_database
                        containerInstance socnet.messages_database
                    }
                }
            }
        }

    }

    views {
        themes default
        systemContext socnet "System_Context" {
            include *
            autoLayout
            exclude "relationship.tag==container_only"
        }

        container socnet "Containers" {
            include *
            autoLayout
        }

        deployment socnet "Production" "Deployment" {
            include *
            description "Типовое размещение оборудования"
            exclude "relationship.tag==container_only"
            autoLayout lr
        }

        dynamic socnet "UC01" "Создание нового пользователя" {
            autoLayout
            user -> socnet.web_app "Создать нового пользователя"
            user -> socnet.mobile_app "Создать нового пользователя"
            
            socnet.web_app -> socnet.web_back "Вызов создания нового пользователя (POST /user...)"
            socnet.mobile_app -> socnet.mobile_back "Вызов создания нового пользователя (POST /user...)"

            socnet.web_back -> socnet.user_database "Сохранить данные о новом пользователе. INSERT INTO users (login, pass, name, lastname) values(..., ..., ..., ...)"
            socnet.mobile_back -> socnet.user_database "Сохранить данные о новом пользователе. INSERT INTO users (login, pass, name, lastname) values(..., ..., ..., ...)"
        }

        dynamic socnet "UC02" "Поиск пользователя по логину" {
            autoLayout
            user -> socnet.web_app "Найти пользователя по логину"
            user -> socnet.mobile_app "Найти пользователя по логину"
            
            socnet.web_app -> socnet.web_back "Вызов поиска пользователя по логину (GET /user..)"
            socnet.mobile_app -> socnet.mobile_back "Вызов поиска пользователя по логину (GET /user..)"

            socnet.web_back -> socnet.user_database "Предоставить данные о пользователя по логину. SELECT * FROM users WHERE login=..."
            socnet.mobile_back -> socnet.user_database "Предоставить данные о пользователя по логину. SELECT * FROM users WHERE login=..."
        }

        dynamic socnet "UC03" "Поиск пользователя по маске имени и фамилии" {
            autoLayout
            user -> socnet.web_app "Найти пользователя по маске имени и фамилии"
            user -> socnet.mobile_app "Найти пользователя по маске имени и фамилии"
            
            socnet.web_app -> socnet.web_back "Вызов поиска пользователя по маске имени и фамилии (GET /user..)"
            socnet.mobile_app -> socnet.mobile_back "Вызов поиска пользователя по маске имени и фамилии (GET /user..)"

            socnet.web_back -> socnet.user_database "Предоставить данные о пользователя по маске имени и фамилии. SELECT * FROM users WHERE name LIKE ... AND lastname LIKE ..."
            socnet.mobile_back -> socnet.user_database "Предоставить данные о пользователя по маске имени и фамилии. SELECT * FROM users WHERE name LIKE ... AND lastname LIKE ..."
        }

        dynamic socnet "UC04" "Добавление записи на стену" {
            autoLayout
            user -> socnet.web_app "Добавить запись на стену"
            user -> socnet.mobile_app "Добавить запись на стену"
            
            socnet.web_app -> socnet.web_back "Вызов создания новой записи на стене (POST /wall..)"
            socnet.mobile_app -> socnet.mobile_back "Вызов создания новой записи на стене (POST /wall..)"

            socnet.web_back -> socnet.wall_database "Сохранить данные о новой записи. INSERT INTO reports (..., ...) values(..., ...)"
            socnet.mobile_back -> socnet.wall_database "Сохранить данные о новой записи. INSERT INTO reports (..., ...) values(..., ...)"
        }

        dynamic socnet "UC05" "Загрузка стены пользователя" {
            autoLayout
            user -> socnet.web_app "Получить все записи на стене пользователя"
            user -> socnet.mobile_app "Получить все записи на стене пользователя"
            
            socnet.web_app -> socnet.web_back "Вызов списка всех записей со стены пользоваетля (GET /wall..)"
            socnet.mobile_app -> socnet.mobile_back "Вызов списка всех записей со стены пользоваетля (GET /wall..)"

            socnet.web_back -> socnet.wall_database "Получить список всех записей со стены пользователя. SELECT * FROM wall WHERE user_id =..."
            socnet.mobile_back -> socnet.wall_database "Получить список всех записей со стены пользователя. SELECT * FROM wall WHERE user_id =..."
        }

        dynamic socnet "UC06" "Отправка сообщения пользователю" {
            autoLayout
            user -> socnet.web_app "Отправить сообщение пользователю"
            user -> socnet.mobile_app "Отправить сообщение пользователю"
            
            socnet.web_app -> socnet.web_back "Вызов создания нового сообщения (POST /messages..)"
            socnet.mobile_app -> socnet.mobile_back "Вызов создания нового сообщения (POST /messages..)"

            socnet.web_back -> socnet.messages_database "Сохранить данные о новом сообщении. INSERT INTO messages (..., ...) values(..., ...)"
            socnet.mobile_back -> socnet.messages_database "Сохранить данные о новом сообщении. INSERT INTO messages (..., ...) values(..., ...)"
        }

        dynamic socnet "UC07" "Получения списка сообщений для пользователя" {
            autoLayout
            user -> socnet.web_app "Получить список сообщений для пользователя"
            user -> socnet.mobile_app "Получить список сообщения для пользователя"
            
            socnet.web_app -> socnet.web_back "Запрос списка сообщений для пользователя (GET /messages..)"
            socnet.mobile_app -> socnet.mobile_back "Запрос списка сообщений для пользователя (GET /messages..)"

            socnet.web_back -> socnet.messages_database "Получить список всех сообщей для пользователя. SELECT * FROM messages WHERE user_id =... AND to_user_id =... "
            socnet.mobile_back -> socnet.messages_database "Получить список всех сообщей для пользователя. SELECT * FROM messages WHERE user_id =... ANDto_user_id =... "
        }

        styles {
            element "database" {
                shape Cylinder
            }

            element "web_app" {
                shape WebBrowser
            }

            element "mobile_app" {
                shape MobileDevicePortrait
            }

            element "Person" {
                shape Person
            }
        }
    }
}