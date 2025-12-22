// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#ifndef AGSHAREDTOS_H
#define AGSHAREDTOS_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVector>
#include <QUuid>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>
#include "cnmea.h"
#include "vec3.h"
#include "ctrack.h"

class CNMEA;

class AgShareDtos
{
public:
    // Базовый DTO для координат
    struct CoordinateDto {
        double latitude;
        double longitude;

        CoordinateDto(double lat = 0, double lon = 0) : latitude(lat), longitude(lon) {}

        QJsonObject toJson() const {
            QJsonObject obj;
            obj["latitude"] = latitude;
            obj["longitude"] = longitude;
            return obj;
        }

        static CoordinateDto fromJson(const QJsonObject& json) {
            return CoordinateDto(
                json["latitude"].toDouble(),
                json["longitude"].toDouble()
                );
        }

        bool isValid() const {
            return !std::isnan(latitude) && !std::isnan(longitude) &&
                   latitude >= -90.0 && latitude <= 90.0 &&
                   longitude >= -180.0 && longitude <= 180.0;
        }
    };

    // DTO для AB-линий и кривых
    struct AbLineUploadDto {
        QString name;
        QString type; // "AB" или "Curve"
        QList<CoordinateDto> coords;

        AbLineUploadDto() : type("AB") {}

        QJsonObject toJson() const {
            QJsonObject obj;
            obj["name"] = name;
            obj["type"] = type;

            QJsonArray coordsArray;
            for (const auto& coord : coords) {
                coordsArray.append(coord.toJson());
            }
            obj["coords"] = coordsArray;

            return obj;
        }

        static AbLineUploadDto fromJson(const QJsonObject& json) {
            AbLineUploadDto dto;
            dto.name = json["name"].toString();
            dto.type = json["type"].toString();

            QJsonArray coordsArray = json["coords"].toArray();
            for (const auto& coordVal : coordsArray) {
                dto.coords.append(CoordinateDto::fromJson(coordVal.toObject()));
            }

            return dto;
        }

        bool isValid() const {
            if (name.isEmpty() || coords.isEmpty()) return false;
            if (type != "AB" && type != "Curve") return false;

            // Проверка координат
            for (const auto& coord : coords) {
                if (!coord.isValid()) return false;
            }

            // Для AB линии нужно 2 точки
            if (type == "AB" && coords.size() != 2) return false;

            // Для кривой минимум 2 точки
            if (type == "Curve" && coords.size() < 2) return false;

            return true;
        }
    };

    // Конвертеры координат с использованием CNMEA (ОБЪЯВЛЕНИЕ - только один раз!)
    class CoordinateConverter {
    public:
        // Конвертация WGS84 в локальные NE координаты через CNMEA
        static Vec3 wgs84ToLocal(const CoordinateDto& wgs84, CNMEA* nmea);

        // Конвертация локальных NE в WGS84 через CNMEA
        static CoordinateDto localToWgs84(const Vec3& local, CNMEA* nmea);

        // Конвертация границы из локальных в WGS84 координаты
        static QList<CoordinateDto> convertBoundaryToWgs84(const QList<Vec3>& localBoundary,
                                                           CNMEA* nmea);

        // Конвертация границы из WGS84 в локальные координаты
        static QList<Vec3> convertBoundaryToLocal(const QList<CoordinateDto>& wgs84Boundary,
                                                  CNMEA* nmea);

        // Создание AB-линии из CTrk
        static AbLineUploadDto createAbLineFromTrack(const CTrk& track, CNMEA* nmea);
    };

    // DTO для поля AgShare (полный объект)
    struct AgShareFieldDto {
        QUuid id;
        QString name;
        bool isPublic;
        QDateTime createdAt;
        double latitude;
        double longitude;
        double convergence;
        QList<QList<CoordinateDto>> boundaries;
        QList<AbLineUploadDto> abLines;

        AgShareFieldDto() : isPublic(false), latitude(0), longitude(0), convergence(0) {}

        QJsonObject toJson() const {
            QJsonObject obj;
            obj["id"] = id.toString();
            obj["name"] = name;
            obj["isPublic"] = isPublic;
            obj["createdAt"] = createdAt.toString(Qt::ISODate);
            obj["latitude"] = latitude;
            obj["longitude"] = longitude;
            obj["convergence"] = convergence;

            // Boundaries
            QJsonArray boundariesArray;
            for (const auto& boundary : boundaries) {
                QJsonArray boundaryArray;
                for (const auto& coord : boundary) {
                    boundaryArray.append(coord.toJson());
                }
                boundariesArray.append(boundaryArray);
            }
            obj["boundaries"] = boundariesArray;

            // AB Lines
            QJsonArray abLinesArray;
            for (const auto& abLine : abLines) {
                abLinesArray.append(abLine.toJson());
            }
            obj["abLines"] = abLinesArray;

            return obj;
        }

        static AgShareFieldDto fromJson(const QJsonObject& json) {
            AgShareFieldDto dto;

            dto.id = QUuid(json["id"].toString());
            dto.name = json["name"].toString();
            dto.isPublic = json.value("isPublic").toBool(false);
            dto.createdAt = QDateTime::fromString(json.value("createdAt").toString(), Qt::ISODate);
            dto.latitude = json["latitude"].toDouble();
            dto.longitude = json["longitude"].toDouble();
            dto.convergence = json.value("convergence").toDouble(0);

            // Boundaries (новая структура с outer и holes)
            QJsonObject boundaryObj = json.value("boundary").toObject();
            if (!boundaryObj.isEmpty()) {
                // Outer boundary
                QJsonArray outerArray = boundaryObj["outer"].toArray();
                if (!outerArray.isEmpty()) {
                    QList<CoordinateDto> outerBoundary;
                    for (const auto& coordVal : outerArray) {
                        outerBoundary.append(CoordinateDto::fromJson(coordVal.toObject()));
                    }
                    if (outerBoundary.size() >= 3) {
                        dto.boundaries.append(outerBoundary);
                    }
                }

                // Holes
                QJsonArray holesArray = boundaryObj["holes"].toArray();
                for (const auto& holeVal : holesArray) {
                    QJsonArray holeArray = holeVal.toArray();
                    QList<CoordinateDto> holeBoundary;
                    for (const auto& coordVal : holeArray) {
                        holeBoundary.append(CoordinateDto::fromJson(coordVal.toObject()));
                    }
                    if (holeBoundary.size() >= 3) {
                        dto.boundaries.append(holeBoundary);
                    }
                }
            } else {
                // Старая структура для обратной совместимости
                QJsonArray boundariesArray = json.value("boundaries").toArray();
                for (const auto& boundaryVal : boundariesArray) {
                    QJsonArray boundaryArray = boundaryVal.toArray();
                    QList<CoordinateDto> boundary;
                    for (const auto& coordVal : boundaryArray) {
                        boundary.append(CoordinateDto::fromJson(coordVal.toObject()));
                    }
                    if (boundary.size() >= 3) {
                        dto.boundaries.append(boundary);
                    }
                }
            }

            // AB Lines
            QJsonArray abLinesArray = json.value("abLines").toArray();
            for (const auto& abLineVal : abLinesArray) {
                AbLineUploadDto abLine = AbLineUploadDto::fromJson(abLineVal.toObject());
                if (abLine.isValid()) {
                    dto.abLines.append(abLine);
                }
            }

            return dto;
        }

        bool isValid() const {
            if (id.isNull() || name.isEmpty()) return false;
            if (!CoordinateDto(latitude, longitude).isValid()) return false;

            // Проверка границ
            for (const auto& boundary : boundaries) {
                if (boundary.size() < 3) return false;
                for (const auto& coord : boundary) {
                    if (!coord.isValid()) return false;
                }
            }

            // Проверка AB линий
            for (const auto& abLine : abLines) {
                if (!abLine.isValid()) return false;
            }

            return true;
        }
    };

    // DTO для списка полей (упрощенный)
    struct AgShareFieldListDto {
        QUuid id;
        QString name;
        QList<CoordinateDto> outerBoundary;
        double areaHa;

        AgShareFieldListDto() : areaHa(0) {}

        QJsonObject toJson() const {
            QJsonObject obj;
            obj["id"] = id.toString();
            obj["name"] = name;
            obj["areaHa"] = areaHa;

            QJsonArray boundaryArray;
            for (const auto& coord : outerBoundary) {
                boundaryArray.append(coord.toJson());
            }
            obj["outerBoundary"] = boundaryArray;

            return obj;
        }

        static AgShareFieldListDto fromJson(const QJsonObject& json) {
            AgShareFieldListDto dto;

            dto.id = QUuid(json["id"].toString());
            dto.name = json["name"].toString();
            dto.areaHa = json.value("areaHa").toDouble(0);

            // Outer boundary
            if (json.contains("outerBoundary")) {
                QJsonArray boundaryArray = json["outerBoundary"].toArray();
                for (const auto& coordVal : boundaryArray) {
                    dto.outerBoundary.append(CoordinateDto::fromJson(coordVal.toObject()));
                }
            }

            return dto;
        }
    };

    // Снапшот локального поля
    struct FieldSnapshot {
        QString fieldName;
        QString fieldDirectory;
        QUuid fieldId;
        double originLat;
        double originLon;
        double convergence;
        QList<QList<Vec3>> boundaries;
        QVector<CTrk> tracks;
        CNMEA* converter;  // Для конвертации координат

        FieldSnapshot() : originLat(0), originLon(0), convergence(0), converter(nullptr) {}

        bool isValid() const {
            if (fieldName.isEmpty() || fieldId.isNull()) return false;
            if (converter == nullptr) return false;
            return true;
        }
    };

    // Результат операции API
    struct ApiResult {
        bool success;
        QString message;
        QJsonObject data;

        ApiResult() : success(false) {}

        static ApiResult fromJson(const QJsonObject& json) {
            ApiResult result;
            result.success = json["ok"].toBool(false);
            result.message = json["message"].toString();
            result.data = json["data"].toObject();
            return result;
        }

        QJsonObject toJson() const {
            QJsonObject obj;
            obj["ok"] = success;
            obj["message"] = message;
            obj["data"] = data;
            return obj;
        }
    };

    // Вспомогательные гео-функции
    class GeoUtils {
    public:
        static double calculateAreaInHa(const QList<CoordinateDto>& polygon) {
            if (polygon.size() < 3) return 0.0;

            double area = 0.0;
            int n = polygon.size();

            for (int i = 0; i < n; i++) {
                int j = (i + 1) % n;
                area += polygon[i].latitude * polygon[j].longitude;
                area -= polygon[j].latitude * polygon[i].longitude;
            }

            area = qAbs(area) / 2.0;

            // Упрощенный расчет (1° ≈ 111 км)
            const double R = 6371000.0;
            const double DEG_TO_RAD = M_PI / 180.0;
            double areaM2 = area * DEG_TO_RAD * DEG_TO_RAD * R * R;

            return areaM2 / 10000.0; // в гектарах
        }

        static double calculateDistance(const CoordinateDto& p1, const CoordinateDto& p2) {
            const double R = 6371000.0;
            const double DEG_TO_RAD = M_PI / 180.0;

            double lat1 = p1.latitude * DEG_TO_RAD;
            double lon1 = p1.longitude * DEG_TO_RAD;
            double lat2 = p2.latitude * DEG_TO_RAD;
            double lon2 = p2.longitude * DEG_TO_RAD;

            double dlat = lat2 - lat1;
            double dlon = lon2 - lon1;

            double a = qSin(dlat/2) * qSin(dlat/2) +
                       qCos(lat1) * qCos(lat2) *
                           qSin(dlon/2) * qSin(dlon/2);
            double c = 2 * qAtan2(qSqrt(a), qSqrt(1-a));

            return R * c;
        }

        static CoordinateDto getPolygonCenter(const QList<CoordinateDto>& polygon) {
            if (polygon.isEmpty()) return CoordinateDto();

            double sumLat = 0.0, sumLon = 0.0;
            for (const auto& point : polygon) {
                sumLat += point.latitude;
                sumLon += point.longitude;
            }

            return CoordinateDto(sumLat / polygon.size(), sumLon / polygon.size());
        }

        static bool isPointInPolygon(const CoordinateDto& point, const QList<CoordinateDto>& polygon) {
            if (polygon.size() < 3) return false;

            bool inside = false;
            int n = polygon.size();

            for (int i = 0, j = n - 1; i < n; j = i++) {
                if (((polygon[i].latitude > point.latitude) != (polygon[j].latitude > point.latitude)) &&
                    (point.longitude < (polygon[j].longitude - polygon[i].longitude) *
                                               (point.latitude - polygon[i].latitude) /
                                               (polygon[j].latitude - polygon[i].latitude) + polygon[i].longitude)) {
                    inside = !inside;
                }
            }

            return inside;
        }
    };

    // Фабрики для создания DTO
    class DtoFactory {
    public:
        // Создание AgShareFieldDto из локального поля
        static AgShareFieldDto createFromLocalField(const FieldSnapshot& snapshot) {
            AgShareFieldDto dto;

            if (!snapshot.isValid()) {
                qWarning() << "AgShare: Invalid field snapshot";
                return dto;
            }

            dto.id = snapshot.fieldId;
            dto.name = snapshot.fieldName;
            dto.isPublic = false;
            dto.createdAt = QDateTime::currentDateTime();
            dto.latitude = snapshot.originLat;
            dto.longitude = snapshot.originLon;
            dto.convergence = snapshot.convergence;

            // Границы
            for (const auto& boundary : snapshot.boundaries) {
                dto.boundaries.append(CoordinateConverter::convertBoundaryToWgs84(boundary, snapshot.converter));
            }

            // AB-линии
            for (const auto& track : snapshot.tracks) {
                if (track.mode == 2 || (track.mode == 4 && !track.curvePts.isEmpty())) {
                    AbLineUploadDto abLine = CoordinateConverter::createAbLineFromTrack(track, snapshot.converter);
                    if (abLine.isValid()) {
                        dto.abLines.append(abLine);
                    }
                }
            }

            return dto;
        }

        // Создание FieldSnapshot из AgShareFieldDto
        static FieldSnapshot createFromAgShareField(const AgShareFieldDto& dto, CNMEA* nmea) {
            FieldSnapshot snapshot;

            if (!dto.isValid() || !nmea) {
                qWarning() << "AgShare: Invalid DTO or converter";
                return snapshot;
            }

            snapshot.fieldId = dto.id;
            snapshot.fieldName = dto.name;
            snapshot.originLat = dto.latitude;
            snapshot.originLon = dto.longitude;
            snapshot.convergence = dto.convergence;
            snapshot.converter = nmea;

            // Установить origin в CNMEA
            nmea->latStart = dto.latitude;
            nmea->lonStart = dto.longitude;

            // Границы
            for (const auto& wgs84Boundary : dto.boundaries) {
                snapshot.boundaries.append(CoordinateConverter::convertBoundaryToLocal(wgs84Boundary, nmea));
            }

            return snapshot;
        }

        // Создание JSON для загрузки поля (совместимый с C#)
        static QJsonObject createUploadPayload(const AgShareFieldDto& dto) {
            QJsonObject payload;

            if (!dto.isValid()) {
                qWarning() << "AgShare: Cannot create payload from invalid DTO";
                return payload;
            }

            // Основная информация
            payload["name"] = dto.name;
            payload["isPublic"] = dto.isPublic;
            payload["convergence"] = dto.convergence;

            // Origin (как в C#)
            QJsonObject origin;
            origin["latitude"] = dto.latitude;
            origin["longitude"] = dto.longitude;
            payload["origin"] = origin;

            // Boundary с outer и holes (как в C#)
            QJsonObject boundary;
            if (!dto.boundaries.isEmpty()) {
                // Outer boundary
                QJsonArray outerArray;
                for (const auto& coord : dto.boundaries.first()) {
                    outerArray.append(coord.toJson());
                }
                boundary["outer"] = outerArray;

                // Holes
                if (dto.boundaries.size() > 1) {
                    QJsonArray holesArray;
                    for (int i = 1; i < dto.boundaries.size(); i++) {
                        QJsonArray holeArray;
                        for (const auto& coord : dto.boundaries[i]) {
                            holeArray.append(coord.toJson());
                        }
                        holesArray.append(holeArray);
                    }
                    boundary["holes"] = holesArray;
                }
            }
            payload["boundary"] = boundary;

            // AB Lines
            QJsonArray abLinesArray;
            for (const auto& abLine : dto.abLines) {
                abLinesArray.append(abLine.toJson());
            }
            payload["abLines"] = abLinesArray;

            // Для совместимости с сервером
            payload["sourceId"] = QJsonValue(); // null

            return payload;
        }

        // Создание JSON для загрузки поля (старая версия)
        static QJsonObject createSimpleUploadPayload(const AgShareFieldDto& dto) {
            return dto.toJson();
        }
    };
};

// Встраиваемые реализации методов CoordinateConverter
inline Vec3 AgShareDtos::CoordinateConverter::wgs84ToLocal(const CoordinateDto& wgs84, CNMEA* nmea) {
    if (!nmea) return Vec3();
    double northing, easting;
    nmea->ConvertWGS84ToLocal(wgs84.latitude, wgs84.longitude, northing, easting);
    return Vec3(easting, northing, 0);
}

inline AgShareDtos::CoordinateDto AgShareDtos::CoordinateConverter::localToWgs84(const Vec3& local, CNMEA* nmea) {
    if (!nmea) return CoordinateDto();
    double lat, lon;
    nmea->ConvertLocalToWGS84(local.northing, local.easting, lat, lon);
    return CoordinateDto(lat, lon);
}

inline QList<AgShareDtos::CoordinateDto> AgShareDtos::CoordinateConverter::convertBoundaryToWgs84(
    const QList<Vec3>& localBoundary, CNMEA* nmea) {
    QList<CoordinateDto> result;
    if (!nmea) return result;

    for (const auto& point : localBoundary) {
        result.append(localToWgs84(point, nmea));
    }

    // Замыкаем полигон если нужно
    if (result.size() > 1 && result.first().latitude != result.last().latitude) {
        result.append(result.first());
    }

    return result;
}

inline QList<Vec3> AgShareDtos::CoordinateConverter::convertBoundaryToLocal(
    const QList<CoordinateDto>& wgs84Boundary, CNMEA* nmea) {
    QList<Vec3> result;
    if (!nmea) return result;

    for (const auto& point : wgs84Boundary) {
        result.append(wgs84ToLocal(point, nmea));
    }
    return result;
}

inline AgShareDtos::AbLineUploadDto AgShareDtos::CoordinateConverter::createAbLineFromTrack(
    const CTrk& track, CNMEA* nmea) {
    AbLineUploadDto abLine;
    if (!nmea) return abLine;

    abLine.name = track.name;

    if (track.mode == 2) { // AB mode
        abLine.type = "AB";
        abLine.coords.append(localToWgs84(Vec3(track.ptA.easting, track.ptA.northing, 0), nmea));
        abLine.coords.append(localToWgs84(Vec3(track.ptB.easting, track.ptB.northing, 0), nmea));
    }
    else if (track.mode == 4 && !track.curvePts.isEmpty()) { // Curve mode
        abLine.type = "Curve";
        for (const auto& point : track.curvePts) {
            abLine.coords.append(localToWgs84(point, nmea));
        }
    }

    return abLine;
}

#endif // AGSHAREDTOS_H
