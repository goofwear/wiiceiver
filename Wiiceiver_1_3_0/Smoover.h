/*
 * (CC BY-NC-SA 4.0) 
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 *
 * WARNING WARNING WARNING: attaching motors to a skateboard is 
 * a terribly dangerous thing to do.  This software is totally
 * for amusement and/or educational purposes.  Don't obtain or
 * make a wiiceiver (see below for instructions and parts), 
 * don't attach it to a skateboard, and CERTAINLY don't use it
 * to zip around with just a tiny, ergonomic nunchuck instead
 * of a bulky R/C controller.
 *
 * This software is made freely available.  If you wish to 
 * sell it, don't.  If you wish to modify it, DO! (and please
 * let me know).  Much of the code is derived from others out
 * there, I've made attributuions where appropriate.
 *
 * http://austindavid.com/wiiceiver
 *  
 * latest software: https://github.com/jaustindavid/wiiceiver
 * schematic & parts: http://www.digikey.com/schemeit#t9g
 *
 * Enjoy!  Be safe! 
 * 
 * (CC BY-NC-SA 4.0) Austin David, austin@austindavid.com
 * 12 May 2014
 *
 */

#ifndef SMOOVER_H
#define SMOOVER_H

  class Smoover {
    private:
      float rise, fall, exp_factor, min_step;
      float last, ceiling;

      /* 
       * very simple exponential smoothing algo...
       * ...
       * uses a "minimum step" and caps @ goal
       */
      float expo(float goal) {
        float increment, expoed;
        increment = (goal - last) * exp_factor;
        if (increment > 0) {
          increment = max(min_step, increment);
          expoed = min(last + increment, goal);
        } else {
          increment = min(-min_step, increment);
          expoed = max(last + increment, goal);
          #ifdef DEBUGGING_SMOOVER
          Serial.print("expo(");
          Serial.print(goal);
          Serial.print("): last=");
          Serial.print(last);
          Serial.print(", incr=");
          Serial.print(increment);
          Serial.print(", exp=");
          Serial.println(expoed);
          #endif
        }

        last = expoed;        
        return expoed;
      } // float expo(float goal)
      
      
    public:
    
      // constructor
      Smoover(float rise_, float fall_, float exp_factor_, float min_step_) {
        rise = rise_; 
        fall = fall_; 
        exp_factor = exp_factor_;
        min_step = min_step_;
        zero();
      } // Smoover(rise, fall, exp_factor, min_step)
      
      
      // zeroes internal vars (e.g. ceiling)
      void zero() {
        last = ceiling = 0;
      } // zero()
      
      
      float smoove(float target, float rate) {
        float goal;
        
        #ifdef DEBUGGING_SMOOVER
        if (ceiling > 0) {
          Serial.print("Smoover::smoove2(");
          Serial.print(target, 4);
          Serial.print(", ");
          Serial.print(rate, 4);
          Serial.print("): c=");
          Serial.print(ceiling, 4);
          Serial.print("; ");
        }
        #endif
        if (target > (ceiling + rate)) {
          // increase ceiling as fast as "rise"; goal is pinned here
          ceiling += rate;
          ceiling = min(ceiling, 1.0);
          goal = ceiling;
          #ifdef DEBUGGING_SMOOVER
          if (ceiling > 0) {
            Serial.print(" ^^ ");
          }
          #endif
        } else if (target < (ceiling - rate)) {
          // decrease ceiling as fast as "fall"
          // target is passed through
          ceiling -= rate;
          ceiling = max(ceiling, 0.0);
          goal = target;
          #ifdef DEBUGGING_SMOOVER
          if (ceiling > 0) {
            Serial.print(" vv ");
          }
          #endif
        } else {
          goal = target;
          #ifdef DEBUGGING_SMOOVER
          if (ceiling > 0) {
            Serial.print(" == ");
          }
          #endif
        }

        float expoed = expo(goal);
        #ifdef DEBUGGING_SMOOVER
        if (ceiling > 0) {
          Serial.print(" => c=");
          Serial.print(ceiling, 4);
          Serial.print(", g=");
          Serial.print(goal, 4);
          Serial.print(", e=");
          Serial.println(expoed, 4);
        }
        #endif
        
        return expoed;
      }
      
      
      // return the smooved value for target
      float smoove(float target) {
        float goal;
        
        #ifdef DEBUGGING_SMOOVER
        if (ceiling > 0) {
          Serial.print("Smoover: c=");
          Serial.print(ceiling, 4);
          Serial.print(", t=");
          Serial.print(target, 4);
        }
        #endif
        if (target > (ceiling + rise)) {
          // increase ceiling as fast as "rise"; goal is pinned here
          ceiling += rise;
          ceiling = min(ceiling, 1.0);
          goal = ceiling;
          #ifdef DEBUGGING_SMOOVER
          if (ceiling > 0) {
            Serial.print(" ^^ ");
          }
          #endif
        } else if (target < (ceiling - fall)) {
          // decrease ceiling as fast as "fall"
          // target is passed through
          ceiling -= fall;
          ceiling = max(ceiling, 0.0);
          goal = target;
          #ifdef DEBUGGING_SMOOVER
          if (ceiling > 0) {
            Serial.print(" vv ");
          }
          #endif
        } else {
          goal = target;
          #ifdef DEBUGGING_SMOOVER
          if (ceiling > 0) {
            Serial.print(" == ");
          }
          #endif
        }

        float expoed = expo(goal);
        #ifdef DEBUGGING_SMOOVER
        if (ceiling > 0) {
          Serial.print(" => c=");
          Serial.print(ceiling, 4);
          Serial.print(", g=");
          Serial.print(goal, 4);
          Serial.print(", e=");
          Serial.println(expoed, 4);
        }
        #endif
        
        return expoed;
      } // float smoove(target)


      void rough(float goal) {        
        #ifdef DEBUGGING_SMOOVER
        if (ceiling > 0) {
          Serial.print("Smoover::rough(");
          Serial.print(goal, 4);
          Serial.print(") => c=");
          Serial.print(ceiling, 4);
        }
        #endif
        
        last = ceiling = goal;
          
        #ifdef DEBUGGING_SMOOVER
        if (ceiling > 0) {
          Serial.print("; now c=");
          Serial.println(ceiling, 4);
        }
        #endif
      } // float rough(goal)

  }; // class Smoove
  
#endif