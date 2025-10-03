#include <Arduino.h>
#include "oven.hpp"

Oven oven;

int step = 0;

// void setupWifi()
// {
//   Serial.print("Connecting Wifi");
//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.println("Connecting to WiFi..");
//   }

//   Serial.println("Connected to the WiFi network");
// }

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("Smart Oven starting"));

  // setupWifi();

  oven.Create();
  Serial.println(F("Oven created"));
  // For quick testing: set both top and bottom to 400°C and switch on
  oven.SetTopDesiredTemperature(400.0);
  oven.SetBottomDesiredTemperature(400.0);
  oven.SwitchOn();
}

void loop()
{
  oven.DoCheck();

  // Simple serial command parser to allow: "oven 350" or "oven off"
  if (Serial.available())
  {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0)
    {
      // Expect commands like: oven 400 OR oven off
  if (line.startsWith("oven"))
      {
        int firstSpace = line.indexOf(' ');
        if (firstSpace == -1)
        {
          Serial.println(F("Usage: oven <temp>|off"));
        }
        else
        {
          String arg = line.substring(firstSpace + 1);
          arg.trim();
          if (arg.equalsIgnoreCase("off"))
          {
            oven.SwitchOff();
            Serial.println(F("Oven switched off"));
          }
          else
          {
            double t = arg.toDouble();
            if (t > 0)
            {
              oven.SetTopDesiredTemperature(t);
              oven.SetBottomDesiredTemperature(t);
              oven.SwitchOn();
              Serial.print(F("Oven set to "));
              Serial.print(t);
              Serial.println(F(" C and switched on"));
            }
            else
            {
              Serial.println(F("Invalid temperature"));
            }
          }
        }
      }
      else if (line.equalsIgnoreCase("status"))
      {
        OvenStatus *st = oven.GetStatus();
        Serial.print(F("Oven status: "));
        Serial.println(st->status);

        Serial.print(F("Top element - status: "));
        Serial.print(oven.topElement->GetStatus());
        Serial.print(F(", set: "));
        Serial.print(oven.topElement->GetDesiredTemperature());
        Serial.print(F(" C, temp: "));
        Serial.print(oven.topElement->GetTemperature());
        Serial.print(F(" C, power: "));
        Serial.print(oven.topElement->GetPower());
        Serial.print(F(", max: "));
        Serial.println(oven.topElement->GetMaxPower());

        Serial.print(F("Bottom element - status: "));
        Serial.print(oven.bottomElement->GetStatus());
        Serial.print(F(", set: "));
        Serial.print(oven.bottomElement->GetDesiredTemperature());
        Serial.print(F(" C, temp: "));
        Serial.print(oven.bottomElement->GetTemperature());
        Serial.print(F(" C, power: "));
        Serial.print(oven.bottomElement->GetPower());
        Serial.print(F(", max: "));
        Serial.println(oven.bottomElement->GetMaxPower());
      }
  else if (line.startsWith("power"))
      {
        int firstSpace = line.indexOf(' ');
        if (firstSpace == -1)
        {
          Serial.println(F("Usage: power <value>"));
        }
        else
        {
          String arg = line.substring(firstSpace + 1);
          arg.trim();
          double p = arg.toDouble();
          if (p >= 0)
          {
            // Set max power on both elements
            oven.SetTopMaxPower(p);
            oven.SetBottomMaxPower(p);
            Serial.print(F("Max power set to "));
            Serial.print(p);
            Serial.println(F(" and applied to both elements"));
          }
          else
          {
            Serial.println(F("Invalid power value"));
          }
        }
      }
      else if (line.equalsIgnoreCase("temp"))
      {
        // Print measured temps, setpoints, power and max power for both elements
        double tTop = oven.topElement->GetTemperature();
        double tBottom = oven.bottomElement->GetTemperature();
        double setTop = oven.topElement->GetDesiredTemperature();
        double setBottom = oven.bottomElement->GetDesiredTemperature();
        double powerTop = oven.topElement->GetPower();
        double powerBottom = oven.bottomElement->GetPower();
        double maxTop = oven.topElement->GetMaxPower();
        double maxBottom = oven.bottomElement->GetMaxPower();

        Serial.print(F("Top Temp: "));
        Serial.print(tTop);
        Serial.print(F(" C (set: "));
        Serial.print(setTop);
        Serial.print(F("), power: "));
        Serial.print(powerTop);
        Serial.print(F(" (max: "));
        Serial.print(maxTop);
        Serial.println(F(")"));

        Serial.print(F("Bottom Temp: "));
        Serial.print(tBottom);
        Serial.print(F(" C (set: "));
        Serial.print(setBottom);
        Serial.print(F("), power: "));
        Serial.print(powerBottom);
        Serial.print(F(" (max: "));
        Serial.print(maxBottom);
        Serial.println(F(")"));
      }
      else if (line.startsWith("top"))
      {
        // Commands: top <temp> | top off | top power <value>
        int firstSpace = line.indexOf(' ');
        if (firstSpace == -1)
        {
          Serial.println(F("Usage: top <temp>|off|power <value>"));
        }
        else
        {
          String arg = line.substring(firstSpace + 1);
          arg.trim();
          if (arg.equalsIgnoreCase("off"))
          {
            oven.topElement->SwitchOff();
            Serial.println(F("Top element switched off"));
          }
          else if (arg.startsWith("power"))
          {
            int s = arg.indexOf(' ');
            if (s == -1)
            {
              Serial.println(F("Usage: top power <value>"));
            }
            else
            {
              String pstr = arg.substring(s + 1);
              pstr.trim();
              double p = pstr.toDouble();
              if (p >= 0)
              {
                oven.SetTopMaxPower(p);
                Serial.print(F("Top max power set to "));
                Serial.println(p);
              }
              else
              {
                Serial.println(F("Invalid power value"));
              }
            }
          }
          else
          {
            double t = arg.toDouble();
            if (t > 0)
            {
              oven.SetTopDesiredTemperature(t);
              oven.topElement->SwitchOn();
              Serial.print(F("Top set to "));
              Serial.print(t);
              Serial.println(F(" C and switched on"));
            }
            else
            {
              Serial.println(F("Invalid temperature"));
            }
          }
        }
      }
      else if (line.startsWith("bottom"))
      {
        // Commands: bottom <temp> | bottom off | bottom power <value>
        int firstSpace = line.indexOf(' ');
        if (firstSpace == -1)
        {
          Serial.println(F("Usage: bottom <temp>|off|power <value>"));
        }
        else
        {
          String arg = line.substring(firstSpace + 1);
          arg.trim();
          if (arg.equalsIgnoreCase("off"))
          {
            oven.bottomElement->SwitchOff();
            Serial.println(F("Bottom element switched off"));
          }
          else if (arg.startsWith("power"))
          {
            int s = arg.indexOf(' ');
            if (s == -1)
            {
              Serial.println(F("Usage: bottom power <value>"));
            }
            else
            {
              String pstr = arg.substring(s + 1);
              pstr.trim();
              double p = pstr.toDouble();
              if (p >= 0)
              {
                oven.SetBottomMaxPower(p);
                Serial.print(F("Bottom max power set to "));
                Serial.println(p);
              }
              else
              {
                Serial.println(F("Invalid power value"));
              }
            }
          }
          else
          {
            double t = arg.toDouble();
            if (t > 0)
            {
              oven.SetBottomDesiredTemperature(t);
              oven.bottomElement->SwitchOn();
              Serial.print(F("Bottom set to "));
              Serial.print(t);
              Serial.println(F(" C and switched on"));
            }
            else
            {
              Serial.println(F("Invalid temperature"));
            }
          }
        }
      }
      else
      {
        Serial.println(F("Unknown command"));
      }
    }
  }

  delay(1000);
}