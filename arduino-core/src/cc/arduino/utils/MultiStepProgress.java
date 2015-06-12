/*
 * This file is part of Arduino.
 *
 * Copyright 2014 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package cc.arduino.utils;

public class MultiStepProgress implements Progress {

  private final double steps;

  private double step;
  private double stepProgress;

  String status;

  public MultiStepProgress(int _steps) {
    steps = _steps;
    step = 0.0;
    stepProgress = 0.0;
  }

  public double getGlobalProgress() {
    return (step + stepProgress) / steps;
  }

  public void stepDone() {
    step += 100.0;
    setProgress(0.0);
  }

  @Override
  public void setProgress(double progress) {
    stepProgress = progress;
  }

  @Override
  public void setStatus(String _status) {
    status = _status;
  }

  @Override
  public double getProgress() {
    return getGlobalProgress();
  }

  @Override
  public String getStatus() {
    return status;
  }
}
