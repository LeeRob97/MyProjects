/*
    Lee Chandler Robertson
    ----------------------

    File Description: The Purpose of this file is to expand on the javascript Date library. It does multiple conversions from string date to numeric 
    or vice versa. It also mimics the php Date() function. Thus allowing for dates to be formatted to the users liking.
*/


Date.prototype.addDays = function(days) {
    var dat = new Date(this.valueOf());
    dat.setDate(dat.getDate() + days);
    return dat;
}
  
  
  function daytype(givendate){
      if(!givendate) return givendate;
      if(isNaN(givendate)){ //actual string date given
          var date = makeDate(givendate);
      }else{ //epoch date given
          var date = ezd_epochtodate(givendate);
      }
      /* Date varibles */
      var today = new Date();
      var gdate = new Date(php_date("%m/%d/%Y", date));
      var tdate = new Date(php_date("%m/%d/%Y", today));
      var longdate = php_date("%m/%d/%Y", date);
      var shorttext = php_date("%l", date);
      
      /* logic and return variables */
      var fromtoday = 0; //used to keep track of how many days we are from today either (+) future or (-) past
      var optiontext = ""; //return value
      
      fromtoday = diffdays(tdate, gdate);
      
      if(fromtoday < 0){
          if(fromtoday === -1){
              optiontext = longdate +" Yesterday";
          }else if(fromtoday >= -7){
              optiontext = longdate +" "+("Last " + shorttext);
          }else{
              optiontext = longdate +" "+php_date("%l", date);
          }
      }else if(fromtoday === 0){
          optiontext = longdate +" Today";
      }else if(fromtoday > 0){
          if(fromtoday === 1){
              optiontext = longdate +" Tomorrow";
          }else if(fromtoday <= 7){
              optiontext = longdate+" "+("Next " + shorttext);
          }else{
              optiontext = longdate +" "+php_date("%l", date);
          }
      }
      
      return optiontext;
  }
  
  
  /**
   * 
   * This function takes a date as input and returns a human-readable, textual 
   * representation of about how much time will ellapse/has ellapsed before/since
   * the Date in Question (diq)
   * 
   * 
   * 
   * @param {Mixed} diq (Date In Question)
   * @returns {String} A textual representation of how far out, or how far in the past the Date in Question falls.
   * 
   * Example:
   * 
   * var today = makeDate();
   * var date_to_check = ezd_adddays(today, 1);
   * var desc = ezd_desc( date_to_check );
   * console.log(desc);
   * 
   * Expected Output:
   * 
   * TOMORROW
   * 
   */
  function ezd_desc( diq ) {
      var date_desc = "";
  //    console.log("date_description()");
  //    console.log("\tdiq (before makeDate()): " + diq);
      diq = makeDate(diq);
  //    console.log("\tdiq (after makeDate()): " + diq);
      var today = makeDate();
      var diff = ezd_diffdays(diq, today);
  //    console.log("today="+today);
  //    console.log("days different=" + diff);
      if ( /* It is today */ diff == 0) {
          date_desc = "TODAY";
      } else {
          if ( /* It's a past date */ diff < 0 ) {
              if ( diff >= -7) {
                  if ( diff == -1 ) {
                      date_desc = "YESTERDAY";
                  } else {
                      date_desc = "LAST " + php_date("%l", diq).toUpperCase();
                  }
              } else /* The DIQ is older than seven days */ {
                  if (diff >= - 12) {
                      date_desc = "~ A WEEK AGO";
                  } else if (diff >= -19) {
                      date_desc = "~ 2 WEEKS AGO";
                  } else if (diff >= -24) {
                      date_desc = "~ 3 WEEKS AGO";
                  } else if (diff >= -50) {
                      date_desc = "~ A MONTH AGO";
                  } else if (diff >= -75) {
                      date_desc = "~ 2 MONTHS AGO";
                  } else if (diff >= -100) {
                      date_desc = "~ 3 MONTHS AGO";
                  } else if (diff >= -135) {
                      date_desc = "~ 4 MONTHS AGO";
                  } else if (diff >= -200) {
                      date_desc = "~ 6 MONTHS AGO";
                  } else if (diff >= -360) {
                      date_desc = "ALMOST A YEAR AGO";
                  } else if (diff >= -360) {
                      date_desc = "OVER A YEAR AGO";
                  } else {
                      date_desc = php_date("%Y", diq);
                  }
              }
          } else {
              if (diff <= 7) {
                  if ( diff == 1) {
                      date_desc = "TOMORROW";
                  } else if (diff <= 6) {
                      date_desc = php_date("%l", diq).toUpperCase();
                  } else if (diff == 7) {
                      date_desc = "NEXT " + php_date("%l", diq).toUpperCase();
                  }
              } else /* We're over 8 days out */ {
                  if (diff <= 12) {
                      date_desc = "IN ~ A WEEK";
                  } else if (diff <= 19) {
                      date_desc = "IN ~ 2 WEEKS";
                  } else if (diff <= 24) {
                      date_desc = "IN ~ 3 WEEKS";
                  } else if (diff <= 50) {
                      date_desc = "IN ~ A MONTH";
                  } else if (diff <= 75) {
                      date_desc = "IN ~ 2 MONTHS";
                  } else if (diff <= 100) {
                      date_desc = "IN ~ 3 MONTHS";
                  } else if (diff <= 135) {
                      date_desc = "IN ~ 4 MONTHS";
                  } else if (diff <= 200) {
                      date_desc = "IN ~ 6 MONTHS";
                  } else if (diff <= 360) {
                      date_desc = "IN ~ A YEAR";
                  } else if (diff <= 360) {
                      date_desc = "IN OVER A YEAR";
                  } else {
                      date_desc = php_date("%Y", diq);
                  }
              }
          }
      }
      return date_desc;
  }
  
  
  
  
  /**
   * Attempts to create a Date object out of the parameter that is passed. 
   * 
   * @param {Mixed} stringdate
   * @returns {Date} Javscript date object
   * 
   * 
   * 
   */
  function makeDate( stringdate ) {
      var current_date = new Date();
      if (arguments.length == 1) {
          var d = null;
          try {
              if (typeof stringdate == "Number" || parseInt(stringdate) > 1000000) {
                  console.log("makeDate(): Interpreting \""+stringdate+"\" as epoch");
                  d = ezd_epochtodate(stringdate);
                  d.setTime(d.getTime() + (current_date.getTimezoneOffset() * 60000));
              } else {
                  /* 
                   * Check to see if date is in YYYY-MM-DD format.  If it is, Javascript
                   * keeps thinking that we're supplying it in GMT time and subtracting
                   * the local timezone offset from the date when we pass it to new Date()
                   * so we get inconsistant date result.  Every date from the database
                   * or locally should be converted to the local user's timezone.
                   * 
                   */
                  
                  dts = stringdate.toString();
                  
                  var date_regex = /\d{4}-\d{2}-\d{2}/;
                  var reg_match = dts.match(date_regex);
                  
                  
  //                console.log("makeDate(): reg_match= " + reg_match);
  
  
  
                  if (!reg_match) {
                      d = new Date(stringdate);
                  } else {
                      var date_array = reg_match[0].split("-");
  //                    console.log("Date passed was \""+stringdate+"\", the date was \""+reg_match+"\"");
                      new_date_string = date_array[1] + "/" + date_array[2] + "/" + date_array[0];
  //                    console.log("makeDate(): Converted date to: \""+new_date_string+"\"");
                      stringdate = stringdate.replace(reg_match[0], new_date_string);
  //                    console.log("makeDate(): stringdate is now \""+stringdate+"\"");
                      d = new Date(stringdate);
                  }
              }
          } catch (e) {
              console.log("makeDate(): Error: " + e.message);
              return null;
          }
      } else {
          var cd_time = current_date.getTime();
          var cd_tzo = current_date.getTimezoneOffset();
          current_date = current_date.setTime( cd_time + ( cd_tzo * 60000) );
  //        return new Date(current_date);
          return new Date();
      }
      return d;
  }
  
  /*
   * diffmonths()
   * 
   * @param {Date} dateobject1
   * @param {Date} dateobject1
   * @returns {int} monthcount+1 //+1 to include the starting month
   * 
   * Calculates the number of months from a starting date to an ending date.
   * 
   * 
   */
  function diffmonths(dateobject1, dateobject2){
      var tempDate = makeDate(dateobject1);
      var end = makeDate(dateobject2);
      var monthCount = 0;
      
      while((tempDate.getMonth()+''+tempDate.getFullYear()) != (end.getMonth()+''+end.getFullYear())) {
          monthCount++;
          tempDate.setMonth(tempDate.getMonth()+1);
      }
      return monthCount+1;
  }
  
  
  /**
   * 
   * Returns how many days "comparison_date" is different from "base_date"
   * 
   * @param {type} base_date
   * @param {type} comparison_date
   * @returns {int} A signed integer representing the difference between dates in days
   */
  function ezd_diffdays( base_date, comparison_date ) {
      try {
          var szbd = php_date("%m/%d/%Y", base_date);
          var szcd = php_date("%m/%d/%Y", comparison_date);
          return (makeDate(szbd).getTime() - makeDate(szcd).getTime()) / 86400000;
      } catch (e) {
          console.log(e.message);
          return 0;
      }
  }
  
  
  
  /* Alias of "diffmonths()" */
  function ezd_diffmonths(dateobject1, dateobject2){
      var tempDate = makeDate(dateobject1);
      var end = makeDate(dateobject2);
      var monthCount = 0;
      
      while((tempDate.getMonth()+''+tempDate.getFullYear()) != (end.getMonth()+''+end.getFullYear())) {
          monthCount++;
          tempDate.setMonth(tempDate.getMonth()+1);
      }
      return monthCount+1;
  }
  
  
  
  /*
   * ezd_addmonths()
   * 
   * @param {Date} dateobject
   * @param {Integer} months
   * @returns Date
   * 
   * Increments the Date object the requested number of months and returns
   * a Date object.
   * 
   * 
   */
  
  function ezd_addmonths( dateobject, months ) {
      var returnDate = "";
      
      if(typeof dateobject == 'number'){
          returnDate = ezd_epochtodate(dateobject);
      }
      else {
          returnDate = makeDate(dateobject);
      }
      
      return new Date(returnDate.setMonth(returnDate.getMonth() + months));
  }
  
  /**
   * Increments the Date object the requested number of days and returns
   * a Date object.
   * 
   * @param {Date} dateobject
   * @param {Integer} days
   * @returns Date
   */
  function ezd_adddays( dateobject, days ) {
      var returnDate = makeDate(dateobject);
      if (!returnDate) return null;
      returnDate.setDate(returnDate.getDate() + days);
      return returnDate;
  }
  
  
  /**
   * 
   * This function takes two dates in almost any format and 
   * converts them internally to the same format.  Then it
   * (disregarding time) compares them  and returns true 
   * if the dates are the same, false if they are not.
   * 
   * @param {Date Format} date1
   * @param {Date Format} date2
   * @returns {Boolean}
   */
  function ezd_date_match( date1, date2 ) {
      if (php_date("%m%d%Y", makeDate(date1)) == 
              php_date("%m%d%Y", makeDate(date2))) return true;
      return false;
  }
  
  
  /**
   * 
   * Returns a date object that is "weeks_to_add" weeks more (or less if
   * "weeks_to_add" is negative) than the "base_date"
   * 
   * @param {Date} basedate
   * @param {Integer} years
   * @returns {Date}
   * 
   */
  function ezd_addweeks( basedate, weeks_to_add) {
      try {
          return ezd_adddays(basedate, ((weeks_to_add)*7));
      } catch (e) {
          return null;
      }
  }
  
  
  /*
   * ezd_addyears()
   * 
   * @param {Date} dateobject
   * @param {Integer} years
   * @returns Date
   * 
   * Increments the Date object the requested number of years and returns
   * a Date object.
   * 
   * 
   */
  function ezd_addyears( dateobject, years ) {
      var returnDate = "";
      
      if(typeof dateobject == 'number'){
          returnDate = ezd_epochtodate(dateobject);
      }
      else {
          returnDate = dateobject;
      }
      
      return new Date(returnDate.setFullYear(returnDate.getFullYear() + years));
  }
  
  /* Returns the number of seconds since Jan, 1 1970 (epoch) */
  function ezd_datetoepoch( datestring ) {
      if(typeof datestring === "object"){
          var epoch = datestring;
      }else{
          var epoch = makeDate(datestring);
      }
      
      epoch = epoch.getTime();
      
      if((epoch>=100000000000)||(epoch<=-100000000000)){ //if milliseconds 
          return (epoch/1000);
          
      }
      else if((epoch>=100000000)||(epoch<=-100000000)){ // if its in seconds
          return (epoch);
      }
  }
  
  
  /* 
   * Returns the number of nanoseconds since Jan, 1 1970, this value can be used
   * to initialized a JS Date object, or used in Date::setTime().  It is basically
   * the epoch time * 1000 because browsers use the higher precision nanoseconds
   * instead of just seconds for some stupid reason...Like web apps need that 
   * level of precision...
   *
   */
  function ezd_datetotime( datestring ) {
      var returntime = makeDate(datestring);
      
      return returntime.getTime();
  }
  
  
  /*
   * ezd_epochtodate()
   * 
   * Converts epoch into a JS Date object.
   * 
   * 
   * @param {int} epoch
   * @returns {Date}
   */
  function ezd_epochtodate( epoch ) {
      if((epoch>=100000000000)||(epoch<=-100000000000)){ //if milliseconds 
          return new Date(epoch);
          
      }
      else if((epoch>=100000000)||(epoch<=-100000000)){ // if its in seconds
          return new Date((epoch*1000));
      }
  }
  
  /*
   * 
   * ezd_mmddyyyy()
   * 
   * Return a formated string of the date like: "10/06/2017"
   * 
   * @param {Date} dateobject
   * @returns String
   * 
   */
  function ezd_mmddyyyy( dateobject ) {
      var tempdate = makeDate(dateobject);
      var tempday = tempdate.getDate();
      var tempmonth = tempdate.getMonth()+1;
      
      if(tempdate.getMonth() < 10){
          tempmonth = ("0" + tempmonth).slice(-2);
      }
      
      if(tempdate.getDate() < 10){
          tempday = ("0" + tempday).slice(-2);
      }
      
      return (tempmonth + "/" + tempday + "/" + tempdate.getFullYear());
  }
  
  
  
  /*
   * 
   * ezd_yyyymmdd()
   * 
   * Return a formated string including the date like: "2017-06-10"
   * 
   * @param {Date} dateobject
   * @returns String
   * 
   */
  function ezd_yyyymmdd( dateobject ) {
      var tempdate = makeDate(dateobject);
      var tempday = tempdate.getDate();
      var tempmonth = tempdate.getMonth()+1;
      
      if(tempdate.getMonth() < 10){
          tempmonth = ("0" + tempmonth).slice(-2);
      }
      
      if(tempdate.getDate() < 10){
          tempday = ("0" + tempday).slice(-2);
      }
      
      return (tempdate.getFullYear() + "-" + tempmonth + "-" + tempday);
  }
  
  
  /*
   * 
   * ezd_yyyymmddhhiiss()
   * 
   * Return a formated string including the date and time
   * like: "2017-06-10 13:02:10"
   * 
   * @param {Date} dateobject
   * @returns String
   * 
   */
  function ezd_yyyymmddhhiiss( dateobject ) {
      var tempdate = makeDate(dateobject);
      var tempday = tempdate.getDate();
      var tempmonth = tempdate.getMonth()+1;
      var temptime = ezd_maketime(tempdate);
      
      if(tempdate.getMonth() < 10){
          tempmonth = ("0" + tempmonth).slice(-2);
      }
      
      if(tempdate.getDate() < 10){
          tempday = ("0" + tempday).slice(-2);
      }
      
      return (tempdate.getFullYear() + "-" + tempmonth + "-" + tempday + " " + temptime);
  }
  
  /*
   * 
   * ezd_maketime()
   * 
   * Return a formated string including the date and time
   * like: "13:02:10"
   * 
   * @param {Date} dateobject
   * @returns String
   * 
   */
  function ezd_maketime( dateobject ) {
      var seconds = dateobject.getSeconds();
      var minutes = dateobject.getMinutes();
      var hour = dateobject.getHours();
      
      
      return (hour+ ":" + minutes + ":" + seconds);
  }
  
  Date.prototype.stdTimezoneOffset = function() {
      var jan = new Date(this.getFullYear(), 0, 1);
      var jul = new Date(this.getFullYear(), 6, 1);
      return Math.max(jan.getTimezoneOffset(), jul.getTimezoneOffset());
  }
  
  /* A JavaScript implementation of PHP's date() function.
   * 
   * I have started it for you.
   * 
   * For more information see: http://php.net/manual/en/function.date.php
   * 
   */
  function php_date( szformat, dateobject ) {
      var outstring = "";
      var iepoch = 0;
      var i = 0;
      var d = dateobject;
  //    console.log("php_date(): dateobject is " + dateobject);
      if ( typeof dateobject == "Number" || parseInt(dateobject) > 1000000) {
          d = makeDate(dateobject);
      }
      for (; i < szformat.length; i++) {
          if (szformat.charAt(i) == '%') {
              switch (szformat.charAt(i+1)) {
                  case 'd': /* Day of the month, 2 digits with leading zeros */
                      outstring+=((d.getDate() < 10)?("0"+d.getDate().toString()):d.getDate().toString());
                      break;
                  case 'D': /* A textual representation of a day, three letters */
                      var shortDay = ['Sun', 'Mon', 'Tue', 'Wed', 'Thur', 'Fri', 'Sat'];
                      outstring+=(shortDay[d.getDay()].toString());
                      break;
                  case 'j': //Date without leading zero
                      outstring+=d.getDate().toString();
                      break;
                  case 'l': //A full textual representation of the day of the week (Sunday...)
                      var longDay = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
                      outstring+=(longDay[d.getDay()].toString());
                      break;
                  case 'N': //iso numeric representation of day of the week (sunday = 7)
                      outstring+=(d.getDay() === 0 ? 7 : d.getDay());
                      break;
                  case 'S': //add suffix to date
                      outstring+=(d.getDate()+(d.getDate() % 10 == 1 && d.getDate() != 11 ? 'st' : (d.getDate() % 10 == 2 && d.getDate() != 12 ? 'nd' : (d.getDate() % 10 == 3 && d.getDate() != 13 ? 'rd' : 'th'))));
                      break;
                  case 'w': //regular representation day of week (sunday = 0)
                      outstring+=d.getDay();
                      break; 
                  case 'z'://gives what the current day is out of 365 days of the year
                      var start = new Date(d.getFullYear(), 0, 0);
                      var diff = (d - start) + ((start.getTimezoneOffset() - d.getTimezoneOffset()) * 60 * 1000);
                      var oneDay = 1000 * 60 * 60 * 24;
                      var day = Math.floor(diff / oneDay);
                      outstring+=day;
                      break;
                  case 'W': //iso-8601 current week number *16 = week 16*
                      var tdt = new Date(d.valueOf());
                      var dayn = (d.getDay() + 6) % 7;
                      tdt.setDate(tdt.getDate() - dayn + 3);
                      var firstThursday = tdt.valueOf();
                      tdt.setMonth(0, 1);
                      if (tdt.getDay() !== 4) {
                       tdt.setMonth(0, 1 + ((4 - tdt.getDay()) + 7) % 7);
                      }
                      outstring += 1 + Math.ceil((firstThursday - tdt) / 604800000);
                      break;
                  case 'F': //A full textual representation of a month, such as January or March
                      var longmonth = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
                      outstring+=(longmonth[d.getMonth()].toString());
                      break;
                  case 'm': //Numeric representation of a month, with leading zeros
                      outstring+=(d.getMonth() < 9 ? '0' : '') + (d.getMonth() + 1); 
                      break;
                  case 'M': //A short textual representation of a month, such as Jan or Mar
                      var shortmonth = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
                      outstring+=(shortmonth[d.getMonth()].toString());
                      break;
                  case 'n': //Numeric representation of a month, without leading zeros
                      outstring+=((d.getMonth()+1).toString());
                      break;
                  case 't': //number of days in a given month
                      outstring+=new Date(d.getFullYear(), (d.getMonth()+1), 0).getDate(); //zero is last parameter because it 0 is the last date
                      break;
                  case 'L': //Whether it's a leap year (returns: true or false)
                      outstring+=(d.getFullYear() % 100 === 0) ? (d.getFullYear() % 400 === 0) : (d.getFullYear() % 4 === 0);
                      break;
                  case 'o': //ISO-8601 week-numbering year.
                      var tdt = new Date(d.valueOf());
                      var dayn = (d.getDay() + 6) % 7;
                      tdt.setDate(tdt.getDate() - dayn + 3);
                      var firstThursday = tdt.valueOf();
                      tdt.setMonth(0, 1);
                      if (tdt.getDay() !== 4){
                          tdt.setMonth(0, 1 + ((4 - tdt.getDay()) + 7) % 7);
                      }
                      outstring+=(1 + Math.ceil((firstThursday - tdt) / 604800000)).toString();
                      break;
                  case 'Y': //A full numeric representation of a year, 4 digits
                      outstring+=d.getFullYear();
                      break;
                  case 'y': //Two digit representation
                      outstring+=('' + d.getFullYear()).substr(2);
                      break;
                  case 'a': //Lowercase Ante meridiem and Post meridiem
                      outstring+=(d.getHours() < 12 ? 'am' : 'pm'); 
                      break;
                  case 'A': //Uppercase Ante meridiem and Post meridiem
                      outstring+=(d.getHours() < 12 ? 'AM' : 'PM');
                      break;
                  case 'B': //Swatch Internet time
                      outstring+=Math.floor((((d.getUTCHours() + 1) % 24) + d.getUTCMinutes() / 60 + d.getUTCSeconds() / 3600) * 1000 / 24);
                      break;
                  case 'g': //12-hour format of an hour without leading zeros
                      outstring+=(d.getHours() % 12 || 12);
                      break;
                  case 'G': //24-hour format of an hour without leading zeros
                      outstring+=d.getHours();
                      break;
                  case 'h': //12-hour format of an hour with leading zeros
                      outstring+=((d.getHours() % 12 || 12) < 10 ? '0' : '') + (d.getHours() % 12 || 12);
                      break;
                  case 'H': //24-hour format of an hour with leading zeros
                      outstring+=(d.getHours() < 10 ? '0' : '') + (d.getHours());
                      break;
                  case 'i': //Minutes with leading zeros
                      outstring+=(d.getMinutes() < 10 ? '0' : '') + (d.getMinutes());
                      break;
                  case 's': //Seconds, with leading zeros
                      outstring+=(d.getSeconds() < 10 ? '0' : '') + (d.getSeconds());
                      break;
                  case 'u': //Microseconds 
                      outstring+=(d.getMilliseconds()*1000);
                      break;
                  case 'V': //Milliseconds
                      outstring+=(d.getMilliseconds());
                      break;
                  case 'e': //Timezone identifier
                      var timezone = d.toTimeString();
                      var tzstr = timezone.split("(");
                      var timezoneid = tzstr[1].toString().replace(")","");
                      outstring+=timezoneid;
                      break;
                  case 'I': //Whether or not the date is in daylight saving time (returns 1 = true, 0 = false)
                      outstring+=(d.getTimezoneOffset() < d.stdTimezoneOffset());                                                                                                                                                    
                      break;
                  case 'O': //Difference to Greenwich time (GMT) in hours
                      outstring+=(-d.getTimezoneOffset() < 0 ? '-' : '+') + (Math.abs(d.getTimezoneOffset() / 60) < 10 ? '0' : '')+ (Math.abs(d.getTimezoneOffset() / 60)) + '00';
                      break;
                  case 'P'://Difference to Greenwich time (GMT) in hours with semi colon
                      outstring+=(-d.getTimezoneOffset() < 0 ? '-' : '+') + (Math.abs(d.getTimezoneOffset() / 60) < 10 ? '0' : '')+ (Math.abs(d.getTimezoneOffset() / 60))+ ":" + '00';
                      break;
                  case 'T': //Timezone abbreviation *same above (case 'e')*
                      var timezone = d.toTimeString();
                      var tzstr = timezone.split("(");
                      var timezoneid = tzstr[1].toString().replace(")","");
                      outstring+=timezoneid;
                      break;
                  case 'Z': //timeoffset in seconds
                      outstring+=(-d.getTimezoneOffset() * 60);
                      break;
                  case 'c': //ISO 8601 date
                      outstring+=d.toISOString();
                      break;
                  case 'r': //RFC 2822 formatted date (Thu, 21 Dec 2000 16:01:07 +0200)
                      var GMT = (-d.getTimezoneOffset() < 0 ? '-' : '+') + (Math.abs(d.getTimezoneOffset() / 60) < 10 ? '0' : '')+ (Math.abs(d.getTimezoneOffset() / 60)) + '00';
                      outstring+=d.toUTCString();
                      outstring = outstring.replace("GMT", GMT);
                      break;
                  case 'U': //epoch time (returned in seconds)
                      outstring+=ezd_datetoepoch(d);
                      break;
                  default:
                      break;
              }
              i = i+1; //increment to ensure "case" character is not included in string
          } 
          else {
              outstring+=szformat.charAt(i);                                                                                                                                                    
          }
      }
      return outstring;
  }
  
  
  
  
  