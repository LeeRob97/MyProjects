function makeDate( stringdate ) {
    var d = new Date(stringdate);
    d.setTime(d.getTime() + (d.getTimezoneOffset() * 60000) );

    return d;
}

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

    return returnDate.setMonth(returnDate.getMonth() + months);
}

/*
 * ezd_adddays()
 *
 * @param {Date} dateobject
 * @param {Integer} days
 * @returns Date
 *
 * Increments the Date object the requested number of days and returns
 * a Date object.
 *
 *
 */
function ezd_adddays( dateobject, days ) {
    var returnDate = "";

    if(typeof dateobject == 'number'){
        returnDate = ezd_epochtodate(dateobject);
    }
    else {
        returnDate = makeDate(dateobject);
    }

    return new Date(returnDate.setDate(returnDate.getDate() + days));
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
        returnDate = makeDate(dateobject);
    }

    return new Date(returnDate.setFullYear(returnDate.getFullYear() + years));
}

/* Returns the number of seconds since Jan, 1 1970 (epoch) */
function ezd_datetoepoch( datestring ) {

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

    console.log(tempdate.getFullYear() + "-" + tempdate.getMonth() + "-" + tempdate.getDate());
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

}


/* A JavaScript implementation of PHP's date() function.
 *
 * I have started it for you.
 *
 * For more information see: http://php.net/manual/en/function.date.php
 *
 */
function php_date( szformat, epoch ) {
    var outstring = "";
    var iepoch = 0;
    var i = 0;
    if ( typeof 'epoch' == "string" ) {
        iepoch = parseInt(epoch);
    } else {
        iepoch = epoch;
    }
    if (isNaN(iepoch)) return "";
    var d = new Date(( iepoch * 1000));
    for (; i < szformat.length; i++) {

        if (szformat.charAt(i) == '%') {
            switch (szformat.charAt(i+1)) {
                case 'd': /* Day of the month, 2 digits with leading zeros */
                    outstring+=((d.getDate() < 10)?("0"+d.getDate().toString()):d.getDate().toString());
                    break;
                case 'D': /* A textual representation of a day, three letters */
                    switch (d.getDay()) {
                        case 0:
                            outstring+="Sun"
                            break;
                        case 1:
                            outstring+="Mon"
                            break;
                        case 2:
                            outstring+="Tue"
                            break;
                        case 3:
                            outstring+="Wed"
                            break;
                        case 4:
                            outstring+="Thu"
                            break;
                        case 5:
                            outstring+="Fri"
                            break;
                        case 6:
                            outstring+="Sat"
                            break;
                    }
                    break;
                case 'j':
                    break;
                case 'l':
                    break;
                case 'N':
                    break;
                case 'S':
                    break;
                case 'w':
                    break;
                case 'z':
                    break;
                case 'W':
                    break;
                case 'F':
                    break;
                case 'm':
                    break;
                case 'M':
                    break;
                case 'n':
                    break;
                case 't':
                    break;
                case 'L':
                    break;
                case 'o':
                    break;
                case 'Y':
                    break;
                case 'y':
                    break;
                case 'a':
                    break;
                case 'A':
                    break;
                case 'B':
                    break;
                case 'g':
                    break;
                case 'G':
                    break;
                case 'h':
                    break;
                case 'H':
                    break;
                case 'i':
                    break;
                case 's':
                    break;
                case 'u':
                    break;
                case 'V':
                    break;
                case 'e':
                    break;
                case 'I':
                    break;
                case 'O':
                    break;
                case 'P':
                    break;
                case 'T':
                    break;
                case 'Z':
                    break;
                case 'c':
                    break;
                case 'r':
                    break;
                case 'U':
                    break;
                default:
                    break;
            }
        } else {
            outstring+=szformat.charAt(i);
        }
    }
    return outstring;
}




